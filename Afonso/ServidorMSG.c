/*
                              Exit code
Main                               0
Init                               1
Sockets, bind, accept, listen      2
recicieve/sendto                   3
Write/read                         4
fork                               5
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

extern int errno;



typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);

void kill_my_baby(int sig, pid_t child){
  kill(child, SIGTERM);
}


int main(void){

  struct hostent *h;
  int fd , ret, addrlen, bufferlen, UPT=0, TPT=0, flag, i;
  char buffer[300], NAME[40], IP[20], MESSAGE[140], AUX[140];
  struct sockaddr_in addr, SI_addr, AUX_addr;
  struct in_addr *a;
  pid_t pid; //VARIAVEL DE NOVO PROCESSO. FORK RULES


  //HANDLER
  void (*old_handler)(int);//interrupt handler
  if((old_handler=signal(SIGPIPE,SIG_IGN))==SIG_ERR)exit(1);//error


  if(gethostname(NAME, 40)==-1){ //VAI BUSCAR O NOME DO HOST
    printf("error: %s\n",strerror(errno));
    exit(1);
  }
  printf("HOST's name: %s\n", NAME);

  if((h=gethostbyname(NAME))==NULL){ //PREENCHE OS VALORES DO HOST ATRAVÉS DO NOME.
    printf("Erro a adquirir dados do HOST\n");
    printf("error: %s\n",strerror(errno));
    exit(1);
  }

  a=(struct in_addr*)h->h_addr_list[0]; // O CAST FAZ COM QUE O PROGRAMA INTERPRETE O VALOR COMO SENDO DE UMA VARIAVEL DO TIPO EVOCADO PREMITINDO ACEDER AOS VALORES DA ESTRUTURA NORMALMENTE. TOP QUEQUE
  printf("Internet address: %s (%08lX)\n",inet_ntoa(*a),(long unsigned int)ntohl(a->s_addr));
  sprintf(IP,"%s", inet_ntoa(*a));


  memset((void*)&SI_addr,(int)'\0',sizeof(SI_addr)); //INICIALIZA O ADDR DE RECEÇÃO DO SI.
  SI_addr.sin_family=AF_INET;
  inet_aton("193.136.138.142", &SI_addr.sin_addr);//  TRANFORMA O IP DO SI EM ADDR
  SI_addr.sin_port=htons(59000);//PORTO ONDE O SI RECEBE



  if((pid=fork())==-1){ //CRIAR OUTRO PROCESSO PARA SEPARAR INTERFACE DE UTILIZADOR DOS CLIENTS DO TERMINAL
    printf("Erro na criação de novo processo\n");
    exit(5);
  }

   //PROCESSO FILHO-> CICLO DOS PEDIDOS DOS CLIENTS.

  if(pid==0){
    if((fd=socket(AF_INET, SOCK_DGRAM,0))==-1){  //CRIA A SOCKET UDP POR ONDE CHEGAM OS PEDIDOS DE LIGAÇÃO DE CLIENTS.
      printf("Erro na criação da principal SOCKET\n");
      exit(2);
    }
    printf("SOCKET CLIENT UDP: SUCESS\n");

    memset((void*)&addr,(int)'\0',sizeof(addr)); //INICIALIZA O ADDR DE RECEÇÃO DE DADOS DE CLIENTS.
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(9000);
    UPT=TPT=9000; //PORTO ONDE VOU RECEBER UDP DOS CLIENTS

    if(bind(fd,(struct sockaddr*)&addr,sizeof(addr))==-1){  //ATRIBUIR À SOCKET O ADDR DE RECEÇÃO.
      printf("Não foi possivel fazer o BIND  da SOCKET\n");
      exit(2);
    }
    printf("BIND CLIENT UDP:   SUCESS\n");

    while(1){
      addrlen=sizeof(addr);
      ret=recvfrom(fd, buffer,300,0,(struct sockaddr*)&AUX_addr, &addrlen);  //RECEBER PEDIDO DE CLIENT.

      //ESTE while IMPEDE QUE UMA MENSAGEM DO SI INTERFIRA COM ESTE PROCESSO.
      while(AUX_addr.sin_addr.s_addr==SI_addr.sin_addr.s_addr){
        ret=recvfrom(fd, buffer,300,0,(struct sockaddr*)&AUX_addr, &addrlen);
      }

      if(ret==-1){  //VERIFICAR A RECEÇÃO DE DADOS.
        printf("A receção de dados falhou, RECVFROM deu erro\n");
        exit(3);
      }else if(ret==0){
        printf("A função RECVFROM funciona mas não recebeu nada, tente outra vez\n");
      }

      if((h=gethostbyaddr(&AUX_addr.sin_addr,sizeof(AUX_addr.sin_addr),AF_INET))==NULL){  //SABER  QUE CLIENT FALOU.
        printf("MAIS UM ERRO\n OLHÒ FOGUETE\n");
      }else{
        printf("sent by [%s:%hu]\n",h->h_name,ntohs(AUX_addr.sin_port));
      }

      sscanf(buffer,"%s ",MESSAGE);  //SEPARAR A MENSAGEM DO CLIENT DO PROTOCOLO.

      flag=0;//DESTINGUIR ENTRE COMANDOS
      if(strcmp(MESSAGE, "PUBLISH")==0){
        flag=1;
        printf("INTRUÇÃO RECEBIDA: %s\n",MESSAGE);
      }
      if(strcmp(MESSAGE,"GET_MESSAGES")==0){
        flag=2;
        printf("INTRUÇÃO RECEBIDA: %s\n",MESSAGE);
      }

      switch(flag){
        case 1:
          memset((void*)AUX, (int)'\0',sizeof(AUX));
          strcpy(AUX, buffer+8);// COPIA O QUE VEM A SEGUIR AO PUBLISH.
          bufferlen=strlen(AUX)+1;// TAMANHO DA STRING MAIS O CARACTER DE TERMINAÇÃO
          printf("\n\nPUBLIQUEIIIII: %s\n\n", AUX);
          break;
        case 2:
          printf("\n\nmostrei mensagens\n\n");
          ret=sendto(fd,AUX,bufferlen,0,(struct sockaddr*)&AUX_addr,addrlen);
          break;
        default: break;
      }
    }
  }





   //PROCESSO PAI-> CICLO DA INTERFACE DE UTILIZADOR.
  printf("CHILD PROCESS Nº: %d\n", pid);

  if((fd=socket(AF_INET, SOCK_DGRAM,0))==-1){  //CRIA A SOCKET UDP POR ONDE CHEGAM OS PEDIDOS DE LIGAÇÃO
    printf("Erro na criação da principal SOCKET\n");
    exit(2);
  }
  printf("SOCKET SI UDP: SUCESS\n");

  memset((void*)&addr,(int)'\0',sizeof(addr)); //INICIALIZA O ADDR DE RECEÇÃO DE DADOS DO SERVIDOR IDENTIDADE.
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=htonl(INADDR_ANY);
  addr.sin_port=htons(8500);
  UPT=TPT=9000; //PORTO ONDE VOU RECEBER UDP E TCP

  if(bind(fd,(struct sockaddr*)&addr,sizeof(addr))==-1){  //ATRIBUIR À SOCKET O ADDR DE RECEÇÃO.
    printf("Não foi possivel fazer o BIND  da SOCKET\n");
    exit(2);
  }
  printf("BIND SI UDP:   SUCESS\n");

  while(1){
    addrlen=sizeof(SI_addr);
    memset((void*)buffer,'\0',sizeof(buffer));
    fgets( buffer, 141, stdin); //ORDEM DO UTILIZADOR.

    flag=0;//DESTINGUIR ENTRE COMANDOS
    if(strcmp(buffer, "join\n")==0){
      flag=1;
      printf("INTRUÇÃO DADA: join\n");
    }
    if(strcmp(buffer,"show_servers\n")==0){
      flag=2;
      printf("INTRUÇÃO DADA:  show_servers\n");
    }
    if(strcmp("show_messages\n", buffer)==0){
      flag=3;
      printf("INTRUÇÃO DADA:  show_messages\n");
    }
    if(strcmp("exit\n", buffer)==0){
      flag=4;
      printf("INTRUÇÃO DADA:  exit\n");
    }


    switch(flag){
      case 1 ://REGISTAR O SERVIDOR NO SI.

          sprintf(buffer,"REG %s;%s;%d;%d", NAME,"192.168.1.97" /*IP*/, UPT, TPT);
          bufferlen=strlen(buffer)+1; // STRLEN NAO CONTA COM O \0 NO FIM DA STRING.
          ret=sendto(fd,buffer,bufferlen,0,(struct sockaddr*)&SI_addr,addrlen);

          if(ret==-1){  //VERIFICAR O ENVIU DE DADOS.
            printf("O enviu de dados falhou, SEND TO deu erro\n");
            exit(3);
          }else if(ret==0){
            printf("A função SEND TO funciona mas não enviou nada, tente outra vez\n");
          }
          break;
      case 2 ://PEDIR A LISTA DE SERVIDORES REGISTADOS NO SI.
        ret=sendto(fd,"GET_SERVERS",11,0,(struct sockaddr*)&SI_addr,addrlen);// ENVIAR O PEDIDO

        if(ret==-1){  //VERIFICAR O ENVIU DE DADOS.
          printf("O enviu de dados falhou, SEND TO deu erro\n");
          exit(3);
        }else if(ret==0){
          printf("A função SEND TO funciona mas não enviou nada, tente outra vez\n");
        }

        addrlen=sizeof(AUX_addr);
        ret=recvfrom(fd, buffer,300,0,(struct sockaddr*)&AUX_addr, &addrlen);  //RECEBER RESPOSTA.

        if(ret==-1){  //VERIFICAR A RECEÇÃO DE DADOS.
          printf("A receção de dados falhou, RECVFROM deu erro\n");
          exit(3);
        }else if(ret==0){
          printf("A função RECVFROM funciona mas não recebeu nada, tente outra vez\n");
        }
        printf("%s\n",buffer);  //IMPRIMIR OS OUTROS SERVIDORES.
        break;

      case 4 : // ENCERRAR O PROGRAMA.
        signal(SIGTERM,(void (*)(int))kill_my_baby);
        printf("Programa encerrado por sua ordem\n");
        close(fd);
        //FALTA FAZER O FREE DAS STRINGS!!!
        exit(0);

        break;
      default : break;
    }
  }
  close(fd);
  exit(0);
}
