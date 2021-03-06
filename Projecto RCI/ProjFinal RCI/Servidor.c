/*
 Exit code
 Main                               0
 Init                               1
 Sockets, bind, accept, listen      2
 recicieve/sendto                   3
 Write/read                         4
 fork                               5
 */

//fd socket udp si, fd1 socket udp client, fd2 socket tcp s.m.


/*
 
 Variables          Descrition
 function_select    function_select = 1 ->pede mensagens a SM e guarda; function_select = 0 apenas guarda mensagens enviadas por SM
 
 
 
 */


#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <time.h>

//#include <event.h>

extern int errno;


int max(int a, int b){
    
    if (a>=b) {
        return a;
    }else{
        return b;
    }
}

//Estrutura que guarda o Endereço da HEAD e TAIL da lista de Mensagens
//E guarda o numero de mensagens guardadas
typedef struct List{
    struct MSG * head;
    struct MSG * tail;
    int size;
    
} List_msg;

//Estrutura que guar as Mensagens e o sei relogio lógico
typedef struct MSG{
    char message[141];
    int clock;
    
    struct MSG * ptr;
    
}msg;

//Função que Regista na Lista as Mensagens
msg * RegMsg(msg * tail, char * message, int clock ){
    msg * new;
    new= (msg*)malloc(sizeof(msg));
    
    
    if  ((((tail->clock)-clock)<0) || (((tail->clock)-clock)==0)){
        new->clock=tail->clock + 1;
    }else{
        new->clock=clock;
    }
    
    
    strcpy(new->message, message);
    new->ptr=NULL;
    
    tail->ptr=new;
    tail=new;
    
    return tail;
}

//Função que remove da Lista as mensagens mais antigas
void RemovMsg(msg * head){
    msg * old, *new;
    
    old = head->ptr;
    new = old->ptr;
    head->ptr=new;
    
    free(old);
    
}
//Função que recebe uma string onde vai escrever TODAS as Mensagens
void PrintMsg(msg* head, char * message){
    
    msg * next;
    next= head->ptr;
    
    strcpy(message, "SMESSAGES\n");
    while (next) {
        strcat(message, next->message);
        next=next->ptr;
    }
    strcat(message, "\n");
}


//Função que recebe uma string onde vai escrever as ULTIMAS N Mensagens
void Print_n_Msg(msg* head, char * message, int n, int size){
    int i;
    msg * next;
    next= head->ptr;
    n=size-n;
    
    for (i=0; i<n; i++) {
        next= next->ptr;
    }
    
    strcpy(message, "MESSAGES\n");
    while (next) {
        strcat(message, next->message);
        next=next->ptr;
    }
}


//FUNÇÃO QUE APAGA LISTA de Mensagens
void free_list(msg *head) {
    msg *prev = head;
    msg *cur = head;
    while(cur) {
        prev = cur;
        cur = prev->ptr;
        free(prev);
    }
}










typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);


//ESTRUTURA QUE GUARDA A INFORMAÇÃO DE OUTROS SERVIDORES DE MENSAGENS
typedef struct peers {
    char name[50];
    char ip[16];
    int socket, tcp, udp;
    struct peers *next;
}peers;

//INICIALIZA A LISTA DE PEERS.
void init_list_peers( struct peers *head, char *info, char *ip, int tpt){
    char s[2]="\n", *AUX;
    struct peers *aux, aux1;
    
    
    AUX=strtok(info, s);
    
    while((AUX=strtok(NULL, s))!=NULL){
        sscanf(AUX,"%[^;];%[^;];%d;%d",aux1.name,aux1.ip,&aux1.udp,&aux1.tcp);
        if(strcmp(aux1.ip, ip)!=0 || aux1.tcp!=tpt ){
            if(head->next==NULL){
                head->next=(struct peers *)malloc(sizeof(struct peers));
                if(head->next==NULL){
                    printf("lista de peers mal criada\n");
                    exit(1);
                }
                aux=head->next;
            }else{
                aux->next=(struct peers*)malloc(sizeof(struct peers));
                if(aux->next==NULL){
                    printf("lista de peers mal criada\n");
                    exit(1);
                }
                aux=aux->next;
            }
            sscanf(AUX,"%[^;];%[^;];%d;%d",aux->name,aux->ip,&aux->udp,&aux->tcp);
            printf("estou a ligar -me a:\n nome %s\n ip %s\n udp %d\ntcp %d\n",aux->name,aux->ip,aux->udp,aux->tcp);
            aux->next=NULL;
        }
        
    }
    
}

//LIGA-SE POR TCP AOS PEERS QUE ESTEJAM DEPOIS DO PONTEIRO ENVIADO POR ARGUMENTO.
void connect_peers(struct peers *head){
    struct peers *aux;
    struct sockaddr_in addr;
    int ret;
    
    aux=head->next;
    
    while(aux!=NULL){
        aux->socket=socket(AF_INET,SOCK_STREAM,0);
        
        
        
        memset((void*)&addr,(int)'\0',sizeof(addr));
        addr.sin_family=AF_INET;
        inet_aton(aux->ip,&addr.sin_addr);
        addr.sin_port=htons(aux->tcp);
        printf("pre coonnect\n");
        
        ret=connect(aux->socket,(struct sockaddr *)&addr, sizeof(addr));
        
        if(ret==-1){
            printf("error: %s\n", strerror(errno));
            //exit(2);
        }
        printf("Fiz um connect\n");
        aux=aux->next;
    }
}

//PREENCHE A ESTRUTURA COM A INFO DO PEER DA SOCKET CORRESPONDENTE.
void get_info_peers(struct peers *head,char *info){
    char s[2]="\n", *AUX;
    struct peers *aux, aux1;
    
    aux=head->next;
    while(aux!=NULL){
        if(strcmp("empty",aux->name)==0){
            AUX=strtok(info, s);
            while((AUX=strtok(NULL, s))!=NULL){
                sscanf(AUX,"%[^;];%[^;];%d;%d",aux1.name,aux1.ip,&aux1.udp,&aux1.tcp);
                if(strcmp(aux1.ip,aux->ip)==0){
                    aux->udp=aux1.udp;
                    aux->tcp=aux1.tcp;
                    strcpy(aux->name,aux1.name);
                }
            }
        }
        aux=aux->next;
    }
}

//REMOVE DA LISTA UM PEER QUE FICOU OFFLINE.
void remove_peer(struct peers *head, struct peers *removal){
    struct peers *aux;
    
    close(removal->socket);
    aux=head;
    while(removal!=aux->next){
        aux=aux->next;
    }
    aux->next=removal->next;
    free(removal);
}


void Get_message_peers(int socket){
    
    char * ptr, buffer[14];
    int nleft, nbytes, nwritten;
    
        stpcpy(buffer, "SGET_MESSAGES\n");
        ptr = buffer;
        nbytes = 14;
        
        nleft = nbytes;                             //WRITE MESSAGE
        while (nleft>0) {
            
            nwritten= write(socket, ptr, nleft);
            
            if (nwritten<=0) {                          //ERROR
                printf("error: %s\n", strerror(errno));
                exit(1);
            }
            nleft-=nwritten;
            ptr+=nwritten;
        }
        
    
    
}

//FUNÇÃO QUE APAGA LISTA de PEERS
void free_list_peers(peers *head) {
    peers *prev = head;
    peers *cur = head;
    while(cur) {
        prev = cur;
        cur = prev->next;
        free(prev);
    }
}




int main(int argc, char** argv){
    
    struct peers *head, *AUX_peers;
    msg *first, *last;
    List_msg * imsg;
    int clock, function_select=0;
    int limit, count, check=0, trash;
    char command[40], messages[160];
    char s[2]="\n";
    char * messages_aux;
    
    struct hostent *h;
    int r;
    int n;
    int m=200;
    unsigned long bufferlen=0;
    int fd ,fd1 ,fd2 , ret, addrlen=0, UPT=0, TPT=0, flag, i, maxfd, REG_DONE=0, newfd, first_time=0, nbytes, nwritten;
    int nleft, nread;
    char SI_IP[20]="192.168.1.3";
    char *ptr, *ptr1;
    char buffer[300], NAME[50], IP[20], MESSAGE[140], AUX[140], allmsg[28500], Client_message[28500];
    struct sockaddr_in UDP_addr, SI_addr, TCP_addr, AUX_addr;
    struct in_addr *a;
    fd_set socket_set;
    
    
    
    //LER OS ARGUMENTOS DA IVOCAÇÃO DO PROGRAMA;
    
    //FIZ UMA RECOLHA DE DADOS COM MAIS PROTECÇÃO
    if (argc==9 || argc==11 || argc==13 || argc==15 || argc==17) {
        limit=argc-2;
        for (count = 1; (count <= 7); count= (count + 2)) {
            if(strcmp("-n",argv[count])==0){
                sprintf(NAME ,"%s", argv[count+1]);
                check++;
            }
            else if(strcmp("-j",argv[count])==0){
                sprintf(IP, "%s", argv[count+1]);
                check++;
            }
            else if(strcmp("-u",argv[count])==0){
                UPT=atoi(argv[count+1]);
                check++;
            }
            else if(strcmp("-t",argv[count])==0){
                TPT=atoi(argv[count+1]);
                check++;
            }
            
        }
        if (check<4) {
            printf("Missing arguments!\nPlease try:\n-n name -j ip -u upt -t tpt\n");
            exit(1);
        }
        for (count=9; (count<=limit); count=(count+2)) {
            if(strcmp("-i",argv[count])==0){
                inet_aton(argv[count+1], &SI_addr.sin_addr);
            }
            else if(strcmp("-p",argv[count])==0){
                SI_addr.sin_port=htonl(atoi(argv[count+1]));
            }
            else if(strcmp("-m",argv[count])==0){
                m =atoi(argv[count+1]);
                if (m>200) {
                    printf("Maximum number of Storeged Messages is 200\n");
                    exit(1);
                }
            }
            else if(strcmp("-r",argv[count])==0){
                r=atoi(argv[count+1]);
            }
            
        }
    }else{
        printf("Incorrect Appliction Calling\n ");
        exit(1);
    }

    //INICIALIZA LISTA DE MENSAGES
    first = (msg *)malloc(sizeof(msg));
    first->clock=0;
    first->ptr=NULL;
    
    last=first->ptr;
    
    
    //INICIALIZA ESTRUTURA MEMORIA DE LISTA DE MENSAGENS
    imsg = (List_msg *)malloc(sizeof(List_msg));
    imsg->head = first;
    imsg->tail = last;
    imsg->size = 0;
    
    
    //HANDLER PARA SIGPIPE ERROR
    void (*old_handler)(int);//interrupt handler
    
    if((old_handler=signal(SIGPIPE,SIG_IGN))==SIG_ERR)exit(1);//error
    
    
    //VAI BUSCAR O NOME DO HOST
    if(gethostname(NAME, 50)==-1){
        printf("error: %s\n",strerror(errno));
        exit(1);
    }
    printf("HOST's name: %s\n", NAME);
    
    //PREENCHE OS VALORES DO HOST ATRAVÉS DO NOME.
    if((h=gethostbyname(NAME))==NULL){
        printf("Erro a adquirir dados do HOST\n");
        printf("error: %s\n",strerror(errno));
        exit(1);
    }
    
    a=(struct in_addr*)h->h_addr_list[0]; // O CAST PERMITE QUE O PROGRAMA TRANSFORME O PONTEIRO DE STRING NUM PONTEIRO DE in_addr. TOP QUEQUE
    printf("Internet address: %s (%08lX)\n",inet_ntoa(*a),(long unsigned int)ntohl(a->s_addr));
    sprintf(IP,"%s", inet_ntoa(*a));
    
    //INICIALIZA O ADDR DE RECEÇÃO DO SI.
    memset((void*)&SI_addr,(int)'\0',sizeof(SI_addr));
    SI_addr.sin_family=AF_INET;
    inet_aton(SI_IP, &SI_addr.sin_addr);//  TRANFORMA O IP DO SI EM ADDR
    SI_addr.sin_port=htons(59000);//PORTO ONDE O SI RECEBE
    
    //VALOR PREDEFINIDO
    UPT=TPT=9000;
    r=10;
    
    
    
    //CRIA A SOCKET UDP POR ONDE COMUNICA COM O S.I.
    if((fd=socket(AF_INET, SOCK_DGRAM,0))==-1){
        printf("Erro na criação da SOCKET\n");
        printf("Error: %s",strerror(errno));
        exit(2);
    }
    printf("SOCKET SI UDP: SUCESS\n");
    
    
    //CRIA A SOCKET UDP POR ONDE CHEGAM OS PEDIDOS DE LIGAÇÃO DE CLIENTS.
    if((fd1=socket(AF_INET, SOCK_DGRAM,0))==-1){
        printf("Erro na criação da SOCKET\n");
        printf("Error: %s",strerror(errno));
        exit(2);
    }
    printf("SOCKET CLIENT UDP: SUCESS\n");
    
    //CRIA A SOCKET TCP POR ONDE VAI RECEBER PEDIDOS DE LIGAÇÃO DE OUTROS SERVIDORES DE MENSAGENS
    if((fd2=socket(AF_INET, SOCK_STREAM,0))==-1){
        printf("Erro na criação SOCKET\n");
        printf("Error: %s",strerror(errno));
        exit(2);
    }
    printf("SOCKET SM TCP: SUCESS\n");
    
    //INICILIZA O ADDR DO SERVIDOR UDP
    memset((void*)&UDP_addr,(int)'\0',sizeof(UDP_addr));
    UDP_addr.sin_family=AF_INET;
    UDP_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    UDP_addr.sin_port=htons(UPT);
    
    //INICIALIZA O ADDR DO SERVIDOR TCP
    memset((void*)&TCP_addr,(int)'\0',sizeof(TCP_addr));
    TCP_addr.sin_family=AF_INET;
    TCP_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    TCP_addr.sin_port=htons(TPT);
    
    //LIGA A SOCKET AO ADDR DO SERVER UDP COM O BIND
    if(bind(fd1,(struct sockaddr*)&UDP_addr,sizeof(UDP_addr))==-1){
        printf("Não foi possivel fazer o BIND  da SOCKET\n");
        printf("Error: %s",strerror(errno));
        exit(2);
    }
    printf("BIND SERVER UDP:   SUCESS\n");
    
    //LIGA A SOCKET AO ADDR DO SERVER TCP COM O BIND
    if(bind(fd2,(struct sockaddr*)&TCP_addr,sizeof(TCP_addr))==-1){
        printf("Não foi possivel fazer o BIND  da SOCKET\n");
        printf("Error: %s",strerror(errno));
        exit(2);
    }
    printf("BIND SERVER TCP:   SUCESS\n");
    
    //INICIALIZA A LISTA DE SERVIDORES DE MENSAGENS.
    head=(struct peers*)malloc(sizeof(struct peers));
    if(head==NULL){
        printf("Memória não alocada\n");
        exit(1);
    }
    head->next=NULL;
    
    //SERVIDOR TCP FICA À ESCUTA.
    flag=listen(fd2,5);
    if(flag==-1){
        printf("Erro na função listen\n");
        printf("Error: %s",strerror(errno));
        exit(1);
    }
    
    
    while(1){
        //LIMPA O SET DE FILE DESCRIPTORS
        FD_ZERO(&socket_set);
        
        //INTRODUZ OS FILE DESCRIPTORS DOS SERVIDORES E TECLADO NO SET.
        FD_SET(0,&socket_set);
        maxfd=0;
        FD_SET(fd,&socket_set);
        maxfd=max(maxfd,fd);
        FD_SET(fd1,&socket_set);
        maxfd=max(maxfd,fd1);
        FD_SET(fd2,&socket_set);
        maxfd=max(maxfd,fd2);
        
        //INTRODUZ AS SOCKETS TCP DOS PEERS NO SET.
        AUX_peers=head->next;
        while(AUX_peers!=NULL){
            FD_SET(AUX_peers->socket,&socket_set);
            maxfd=max(maxfd, AUX_peers->socket);
            AUX_peers=AUX_peers->next;
        }
        AUX_peers=NULL;
        
        //ESPERA POR QUALQUER INTERAÇÃO COM AS SOCKETS OU TECLADO (FILE DESCRIPTORS)
        i=select(maxfd+1, &socket_set,(fd_set*)NULL,(fd_set*)NULL, (struct timeval*)NULL);
        
        if(i<=0){
            printf("error: %s\n", strerror(errno));
            exit(1);
        }
        
        //IF QUE PROCESSA ESCRITA NA INTERFACE
        if(FD_ISSET(0, &socket_set)){
            memset((void*)buffer,'\0',sizeof(buffer));
            fgets( buffer, 141, stdin); //ORDEM DO UTILIZADOR.
            sscanf(buffer, "%s %d",command,&trash);
            flag=5;//DESTINGUIR ENTRE COMANDOS
            if(strcmp(command, "join")==0){
                flag=1;
                if (strlen(buffer)>5) {                                //ERROR COMMAND CALL
                    printf("Bad Command Call!\nPlease try: join\n");
                    flag = 0;
                }
                
                printf("INTRUÇÃO DADA: join\n");
            }else if(strcmp(command,"show_servers")==0){
                flag=2;
                if (strlen(buffer)>13) {                                //ERROR COMMAND CALL
                    printf("Bad Command Call!\nPlease try: show_servers\n");
                    flag = 0;
                }
                
                printf("INTRUÇÃO DADA:  show_servers\n");
            }else if(strcmp("show_messages", command)==0){
                flag=3;
                if (strlen(buffer)>14) {                                //ERROR COMMAND CALL
                    printf("Bad Command Call!\nPlease try: show_messages\n");
                    flag = 0;
                }
                
                printf("INTRUÇÃO DADA:  show_messages\n");
            }else if(strcmp("exit", command)==0){
                flag=4;
                if (strlen(buffer)>5) {                                //ERROR COMMAND CALL
                    printf("Bad Command Call!\nPlease try: exit\n");
                    flag = 0;
                }
                
                printf("INTRUÇÃO DADA:  exit\n");
            }
            
            if (flag==5) {
                printf("Command Not Found!\nPlease try:\njoin\nshow_servers\nshow_messages\nexit\n");
            }
            
            //DIRECIONA PARA O COMANDO CERTO E EXECUTA
            switch(flag){
                case 1 ://REGISTAR O SERVIDOR NO SI.
                    //DA PRIMEIRA VEZ QUE FAZ JOIN INICIALIZA A LISTA DE PEERS.
                    if(REG_DONE==0){
                        addrlen=sizeof(SI_addr);
                        ret=sendto(fd,"GET_SERVERS",11,0,(struct sockaddr*)&SI_addr,addrlen);// ENVIAR O PEDIDO
                        
                        if(ret==-1){  //VERIFICAR O ENVIU DE DADOS.
                            printf("O enviu de dados falhou, SEND TO deu erro\n");
                            printf("error: %s\n", strerror(errno));
                            exit(3);
                        }else if(ret==0){
                            printf("A função SEND TO funciona mas não enviou nada, tente outra vez\n");
                        }
                        REG_DONE=1;
                        
                        addrlen=sizeof(SI_addr);
                        sprintf(buffer,"REG %s;%s;%d;%d", NAME, IP, UPT, TPT);
                        bufferlen=strlen(buffer)+1; // STRLEN NAO CONTA COM O \0 NO FIM DA STRING.
                        ret=sendto(fd,buffer,bufferlen,0,(struct sockaddr*)&SI_addr,addrlen);
                        
                        if(ret==-1){  //VERIFICAR O ENVIU DE DADOS.
                            printf("O enviu de dados falhou, SEND TO deu erro\n");
                            printf("error: %s\n", strerror(errno));
                            exit(3);
                        }
                        else if(ret==0){
                            printf("A função SEND TO funciona mas não enviou nada, tente outra vez\n");
                        }
                        
                    break;
                case 2 ://PEDIR A LISTA DE SERVIDORES REGISTADOS NO SI.
                    AUX_peers=head->next;
                    
                    printf("SERVERS \n");
                    while(AUX_peers){
                        printf("%s;%s;%d;%d\n",AUX_peers->name,AUX_peers->ip,AUX_peers->udp,AUX_peers->tcp);
                        AUX_peers=AUX_peers->next;
                    }
                    break;
                case 3 :
                    
                    
                    PrintMsg(imsg->head, allmsg);
                    printf("%s",allmsg);
                    
                    break;
                    
                case 4 : // ENCERRAR O PROGRAMA.
                    printf("Programa encerrado por sua ordem\n");
                    
                    //ENCERRA TODAS AS SOCKETS DOS SERVIDORES DE MENSAGENS.
                    AUX_peers=head->next;
                    while(AUX_peers!=NULL){
                        close(AUX_peers->socket);
                        AUX_peers=AUX_peers->next;
                    }
                    
                        free_list_peers(head);
                        
                        free_list(imsg->head);
                    
                    //LIBERTAR RECURSOS ALOCADOS.(MEMÓRIA E SOCKETS)
                    close(fd);
                    close(fd1);
                    close(fd2);
                    
                    
                    
                    //FALTA FAZER O FREE DAS STRINGS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    
                    
                    
                    exit(0);
                    
                    break;
                default : break;
            }
        }
    }
        if(FD_ISSET(fd, &socket_set)){
            function_select=1;
            addrlen=sizeof(AUX_addr);
            ret=recvfrom(fd, buffer,300,0,(struct sockaddr*)&AUX_addr, &addrlen);
            
            //ERROR
            if(ret==-1){
                printf("error: %s\n", strerror(errno));
                exit(1);
            }
            
            if(REG_DONE==1 && first_time==0){
                first_time=1;
                //Inicializa a Lista de Servidores
                init_list_peers(head, buffer, IP, TPT);
                //Conectar aos Servidores
                connect_peers(head);
                
                Get_message_peers(head->next->socket);
                
            }
            else {
                get_info_peers(head, buffer);
            }
            
            
            function_select=0;
            //VERIFICAÇÃO DO TAMANHO DA LISTA
            if (imsg->size>m) {
                RemovMsg(imsg->head);
                imsg->size--;
            }
            
            
        }
        // IF QUE PROCESSA OS PEDIDOS DOS TERMINAIS/CLIENTS
        if(FD_ISSET(fd1, &socket_set)){
            addrlen=sizeof(AUX_addr);
            memset((void*)buffer, (int)'\0',160);
            ret=recvfrom(fd1, buffer,300,0,(struct sockaddr*)&AUX_addr, &addrlen);  //RECEBER PEDIDO DE CLIENT.
            
            // VERIFICAR RECEÇÃO DE DADOS.
            if(ret==-1){
                printf("A receção de dados falhou, RECVFROM deu erro\n");
                printf("error: %s\n", strerror(errno));
                exit(3);
            }
            
            //SABER  QUE CLIENT FALOU.
            if((h=gethostbyaddr(&AUX_addr.sin_addr,sizeof(AUX_addr.sin_addr),AF_INET))==NULL){
                printf("error: %s\n", strerror(errno));
            }else{
                printf("sent by [%s:%hu]\n",h->h_name,ntohs(AUX_addr.sin_port));
            }
            
            //SEPARAR A MENSAGEM DO CLIENT DO PROTOCOLO.
            sscanf(buffer,"%s %d",MESSAGE,&n);
            
            flag=0;//DESTINGUIR ENTRE COMANDOS
            if(strcmp(MESSAGE, "PUBLISH")==0){
                flag=1;
                printf("INTRUÇÃO RECEBIDA: %s\n",MESSAGE);
            }else if(strcmp(MESSAGE,"GET_MESSAGES")==0){
                flag=2;
                printf("INTRUÇÃO RECEBIDA: %s\n",MESSAGE);
            }
            switch(flag){
                case 1:
                    clock = 0;
                    memset((void*)AUX, (int)'\0',sizeof(AUX));
                    strncpy(AUX, buffer+8,139);// COPIA O QUE VEM A SEGUIR AO PUBLISH.
                   // strncpy()
                    bufferlen=strlen(AUX)+1;// TAMANHO DA STRING MAIS O CARACTER DE TERMINAÇÃO
                    
                    //GUARDA NA LISTA
                    
                    if (imsg->tail==NULL) {
                        imsg->tail=RegMsg(imsg->head, AUX, clock);
                        imsg->size++;
                    }else{
                        imsg->tail=RegMsg(imsg->tail, AUX, clock);
                        imsg->size++;
                    }
                    //VERIFICAÇÃO DO TAMANHO DA LISTA
                    
                    if (imsg->size>m) {
                        RemovMsg(imsg->head);
                        imsg->size--;
                    }
                    
                    
                    //FAZER BROADCAST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    memset((void*)buffer, (int)'\0',160);
                    sprintf(buffer,"SMESSAGES\n%d;%s\n", imsg->tail->clock, imsg->tail->message);
                    
                    //A PARTIR DAQUI ATÈ AO BREAK JÁ TA FEITO.
                    AUX_peers=head->next;
                    while(AUX_peers!=NULL){
                        nleft=strlen(buffer);
                        ptr=buffer;
                        while(nleft>0){
                            nwritten=write(AUX_peers->socket,ptr,nleft);
                            nleft-=nwritten;
                            ptr+=nwritten;
                            if(nwritten<=0){//error
                                printf("O SERVIDOR: %s\nSOCKET: %d\nIP: %s\nPORTO: %d\n NÂO SE ENCONTRA DISPONIVEL", AUX_peers->name,AUX_peers->socket,AUX_peers->ip,AUX_peers->tcp);
                                remove_peer(head, AUX_peers);
                                break;                            }
                        }
                        AUX_peers=AUX_peers->next;
                        
                    }
                    break;
                case 2:
                    
                    //FALTA RECEBER O NUMERO DE MENSAGENS!!!!!!!!!!!!!!!!!!!!
                    printf("\n\nmostrei mensagens\n\n");
                    addrlen=sizeof(AUX_addr);
                    
                    Print_n_Msg(imsg->head, Client_message, n, imsg->size);
                   
                    bufferlen=strlen(Client_message);
                    
                    ret=sendto(fd1,Client_message,bufferlen,0,(struct sockaddr*)&AUX_addr,addrlen);
                    
                    if(ret==-1){  //VERIFICAR O ENVIU DE DADOS.
                        printf("error: %s\n", strerror(errno));
                        exit(3);
                    }
                    
                    break;
                default: break;
            }
        }
        // IF QUE PROCESSA OS PEDIDOS DE OUTROS S.M. PARA SE LIGAREM POR TCP.
        if(FD_ISSET(fd2, &socket_set)){
            addrlen=sizeof(AUX_addr);
            
            if((newfd=accept(fd2,(struct sockaddr *)&AUX_addr, &addrlen)==-1)){
                printf("Conexão TCP rejeitada error: %d", errno);
                printf("error: %s\n", strerror(errno));
                
            }
            
            //PERCORRE A LISTA ATÉ AO FIM
            AUX_peers=head;
            while(AUX_peers->next!=NULL){
                AUX_peers=AUX_peers->next;
            }
            //ALOCA MEMORIA PARA UM PEER.
            AUX_peers->next=(struct peers *)malloc(sizeof(struct peers));
            if(AUX_peers->next==NULL){
                printf("erro de memoria");
            }
            AUX_peers=AUX_peers->next;
            AUX_peers->next=NULL;
            
            //PREENCHE A ESTRUTURA COM OS DADOS DO PEER DISPONIVEIS: IP
            strncpy(AUX_peers->name,"empty\0",6);
            AUX_peers->socket=newfd;
            strcpy(AUX_peers->ip,inet_ntoa(AUX_addr.sin_addr));
            addrlen=sizeof(SI_addr);
            
            //PEDE INFORMAÇÃO AO SI PARA CONFIRMAR O PEER.
            ret=sendto(fd,"GET_SERVERS",11,0,(struct sockaddr*)&SI_addr,addrlen);// ENVIAR O PEDIDO
            
            if(ret==-1){  //VERIFICAR O ENVIU DE DADOS.
                printf("O enviu de dados falhou, SEND TO deu erro\n");
                printf("error: %s\n", strerror(errno));
                exit(3);
            }else if(ret==0){
                printf("A função SEND TO funciona mas não enviou nada, tente outra vez\n");
            }
            
            
        }
            
            //DEPOIS DE SE REGISTAR UMA VEZ COM O S.I. FAZ REGISTOS PERIODICOS
        if(REG_DONE==1){
            addrlen=sizeof(SI_addr);
            sprintf(buffer,"REG %s;%s;%d;%d", NAME, IP, UPT, TPT);
            bufferlen=strlen(buffer)+1; // STRLEN NAO CONTA COM O \0 NO FIM DA STRING.
            ret=sendto(fd,buffer,bufferlen,0,(struct sockaddr*)&SI_addr,addrlen);
                
            if(ret==-1){  //VERIFICAR O ENVIU DE DADOS.
                printf("O enviu de dados falhou, SEND TO deu erro\n");
                printf("error: %s\n", strerror(errno));
                exit(3);
            }
        }
            //PRECORRE A LISTA DE SOCKETS TCP PARA LER.
        AUX_peers=head->next;
        while(AUX_peers!=NULL){
            if(FD_ISSET(AUX_peers->socket, &socket_set)){
                    flag=0;
                    nbytes=160;
                    ptr=messages;
                    nleft=nbytes;
                    
                    while(nleft>0){
                        nread=read(fd,ptr,nleft);
                        if(nread==-1){      //error
                            printf("error: %s\n", strerror(errno));
                            exit(1);
                        }else if(nread==0){ //closed by peer
                            break;
                        }
                        
                        for(i=0; i<nread; i++){
                            ptr1=ptr+i;
                            if(strncmp(ptr1,"\n",1)==0){
                                flag++;
                                if(flag==2)nleft=-1000;
                            }
                            else{
                                flag=0;
                            }
                        }
                        nleft-=nread;
                        ptr+=nread;
                    }
                    
                    //PROCESSA A MENSAGEM RECEBIDA POR BROADCAST
                    
                    if ((strncmp("SMESSAGES\n",messages,10)==0)) {
                        
                        messages_aux=strtok(messages, s);
                        
                        while ((messages_aux=strtok(NULL, s))!=NULL) {
                                                                        //SAI SE CHEGOU AO FIM DO FICHEIRO
                            if ((sscanf("%d;%s", &clock, messages)==0)) {
                                break;
                            }
                            
                            strcat(messages, s);
                            
                            if (imsg->tail==NULL) {
                                imsg->tail=RegMsg(imsg->head, messages, clock);
                                imsg->size++;
                            }else{
                                imsg->tail=RegMsg(imsg->tail, messages, clock);
                                imsg->size++;
                            }
                            //VERIFICAÇÃO DO TAMANHO DA LISTA
                            
                            if (imsg->size>m) {
                                RemovMsg(imsg->head);
                                imsg->size--;
                            }
                        }
                    }
                                                        //RESPONDE AO SERVIDOR COM TODAS AS MENSAGENS
                if ((strncmp("SGET_MESSAGES\n",messages,14)==0)) {
                    PrintMsg(imsg->head, allmsg);
                    ptr = allmsg;
                    nbytes = strlen(allmsg);
                    
                    nleft = nbytes;                             //WRITE MESSAGE
                    while (nleft>0) {
                        
                        nwritten= write(socket, ptr, nleft);
                        
                        if (nwritten<=0) {                          //ERROR
                            printf("error: %s\n", strerror(errno));
                            exit(1);
                        }
                        nleft-=nwritten;
                        ptr+=nwritten;
                    }
                    
                }
            }
            AUX_peers=AUX_peers->next;
        }
        
    }
        //ENCERRA TODAS AS SOCKETS
        AUX_peers=head->next;
        while(AUX_peers!=NULL){
            close(AUX_peers->socket);
            AUX_peers=AUX_peers->next;
        }
        
        //LIBERTAR MEMORIA ALOCADA NA LISTA DE PEERS.
            free_list_peers(head);
            
        //LIBERTAR MEMORIA ALOCADA NA LISTA MENSAGENS
            free_list(imsg->head);
            
        //LIMPAR A LISTA DE MENSAGENS;
        close(fd);
        close(fd1);
        close(fd2);
        exit(0);
    
  }

