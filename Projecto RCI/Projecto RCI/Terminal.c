//
//  Terminal.c
//  Projecto RCI
//
//  Created by Rodrigo Lopes Do Ó Barbosa on 11/03/17.
//  Copyright (c) 2017 Rodrigo Lopes Do Ó Barbosa. All rights reserved.
//

/*
                Exit code
Main                0
Init                1
Sockets             2
recicieve/sendto    3
Write/read          4
 
 Variables          Descrition
 SI_Sock            Socket de Servidor de Identidades
 SI_port            Porto de Servidor de Identidades
 SI_ip              IP do Servidor de Identidades
 SI_addr            Address de Servidor de Identidades
 SM_port            Porto de Servidor de Mensagens
 SM_ip              IP do Servidor de Mensagens
 SM_addr            Address de Servidor de Mensagens
 n_msg              number of messages
 n_charecters       number of charecters
 message            message to publish from user
 command            command from user
 show_msg           requirement to show n messages
 publish_msg        requirement tu publish message
 return_msg         Message return from Servers
 */

# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <errno.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <string.h>
# include <unistd.h>
# include <signal.h>
# include <ctype.h>
extern int errno;

int main(int argc, /*const*/ char * argv[]) {
    
    int SI_port=59000,SM_port;
    int count, limit, flag;
    char SI_ip [16], SM_ip[16];
    int SI_Sock, SM_Sock, n, addrlen;
    char buffer[300];
    char return_msg[300];
    struct sockaddr_in SI_addr;
    struct sockaddr_in SM_addr;
   
    struct in_addr iSI_addr;
    struct in_addr iSM_addr;
    
    char command[40];
    int n_msg;
    
    char *aux_message, message[141], *publish_msg, show_msg[141], aux_show_msg[141];
    unsigned long n_charecters;
    
    
      strcpy(SI_ip, "192.168.1.3");
    
                                                    //TRATAMENTO DE DADOS DE ENTRADA
     if (argc == 1 || argc == 3 || argc == 5 ) {
         limit=argc-2;
        for (count = 1; (count <= limit); count= (count + 2)) {
            if ((strcmp(argv[count], "-i"))==0) {
                strcpy(SI_ip, argv[count+1]);
            }else if ((strcmp(argv[count], "-p"))==0){
                SI_port = atoi(argv[count+1]);
            }else if(((strcmp(argv[count], "-i"))!=0 ) || ((strcmp(argv[count], "-p"))!=0)){
                printf("Incorrect Appliction Calling\n ");
             exit(1);
            }

        }
     }else{
         printf("Incorrect Appliction Calling\n ");
         exit(1);
     }
    
                                //IP CONVERSION SERVIDOR DE IDENTIDADES
    inet_aton(SI_ip,&iSI_addr);
    
    SI_Sock=socket(AF_INET,SOCK_DGRAM,0);          //UDP SOCKET SERVIDOR IDENTIDADES
    
    if(SI_Sock==-1){                                 //ERROR
        printf("error: %s\n", strerror(errno));
        exit(1);
    }
    
    memset((void*)&SI_addr, (int)'\0', sizeof(SI_addr));
    SI_addr.sin_family=AF_INET;
    SI_addr.sin_addr= iSI_addr;
    SI_addr.sin_port=htons(SI_port);
    
    
    strcpy(SM_ip, "192.168.1.3");
    inet_aton(SM_ip,&iSM_addr);
    SM_port = 9000;
    
    SM_Sock=socket(AF_INET,SOCK_DGRAM,0);          //UDP SOCKET SERVIDOR IDENTIDADES
    
    if(SM_Sock==-1){                                 //ERROR
        printf("error: %s\n", strerror(errno));
        exit(1);
    }
    
    memset((void*)&SM_addr, (int)'\0', sizeof(SM_addr));
    SM_addr.sin_family=AF_INET;
    SM_addr.sin_addr= iSM_addr;
    SM_addr.sin_port=htons(SM_port);
    
    
                                        //CICLO DA INTERFACE DE UTILIZADOR.
    
    while(1){
        addrlen=sizeof(SI_addr);
        
        fgets( buffer, 300, stdin);         //ORDEM DO UTILIZADOR.
        sscanf(buffer, "%s %d", command, &n_msg);
        
        flag=5;                 //DESTINGUIR ENTRE COMANDOS E ALOCAR ARGUMENTOS
        
                                                    //SHOW_SERVERS
        if((strcmp("show_servers", command))==0){
            flag=1;
            
            if (strlen(buffer)>13) {                                //ERROR COMMAND CALL
                printf("Bad Command Call!\nPlease try: show_servers\n");
                flag = 0;
            }
        }
                                                //PUBLISH
        if((strcmp("publish", command))==0){
            
            if (strlen(buffer)<9) {
                printf("Please Try: pubish 'message you want to send'\n");
                flag=0;
            }else{
                aux_message = strstr(buffer, " ");
                n_charecters = strlen(aux_message);         //VERIFICAR NUMERO DE CARACTERES
                
                if (n_charecters <= 142) {
                    strcpy(message, (aux_message+1));
                    n_charecters--;
                    flag=2;
                }else{
                    printf("Too many characters!\nMaximum number of charecters is 140.\n");
                    flag = 0;
                }

            }
            
            
        }
                                                        //SHOW_LATEST_MESSAGES
        if(strcmp("show_latest_messages", command)==0){
            
            if (n_msg>0 && n_msg<=200) {
                flag=3;
            }else if((n_msg == 0)){                      //VERIFICAR SE É INTEIRO
            printf("Decimal number expected!\nMaximum number of messages is 200\n");
                flag = 0;
            }else{                                      //VERIFICAR NUMERO DE MENSAGENS
                printf("Too many messages!\nMaximum number of messages is 200.\n");
                flag = 0;
            }
            
        }
                                            //EXIT
        if(strcmp("exit", command)==0){
            flag=4;
            if (strlen(buffer)>5) {                                //ERROR COMMAND CALL
                printf("Bad Command Call!\nPlease try: exit\n");
                flag = 0;
            }
        }
        if (flag==5) {
            printf("Command Not Found!\nPlease try:\nshow_servers\npublish\nshow_latest_messages\nexit\n");
        }
        
        
        switch(flag){
            case 1 ://MOSTRAR SERVIDORES
                
                n=sendto(SI_Sock,"GET_SERVERS",11,0,(struct sockaddr*)&SI_addr, sizeof(SI_addr));
                
                if(n==-1){                                  //ERROR
                    printf("error: %s\n", strerror(errno));
                    exit(1);
                }
                
                addrlen=sizeof(SI_addr);
                
                n= recvfrom(SI_Sock,return_msg ,1000,0,(struct sockaddr*)&SI_addr, &addrlen);
                
                if(n==-1){                                      //ERROR
                    printf("error: %s\n", strerror(errno));
                    exit(1);
                }
                printf("%s\n",return_msg);
                
                break;
            case 2 ://PUBLICAR MENSAGENS
                
                n_charecters = n_charecters+8;              //PREPARAR PROTOCOLO
                publish_msg =(char*) malloc(n_charecters);
                
                strcpy(publish_msg, "PUBLISH ");
                strcat(publish_msg, message);
            
                n=sendto(SM_Sock,publish_msg, n_charecters ,0,(struct sockaddr*)&SM_addr, sizeof(SM_addr));
                
                if(n==-1){                                  //ERROR
                    printf("error: %s\n", strerror(errno));
                    exit(1);
                }
                
                break;
            case 3://MOSTRAR N MENSAGENS
                
                strcpy(show_msg, "GET_MESSAGES ");          //PREPARAR PROTOCOLO
                sprintf(aux_show_msg, "%d", n_msg);
                strcat(show_msg, aux_show_msg);
                n_charecters= strlen(show_msg);
                
                n=sendto(SM_Sock,show_msg, n_charecters ,0,(struct sockaddr*)&SM_addr, sizeof(SM_addr));
                
                if(n==-1){                                  //ERROR
                    printf("error: %s\n", strerror(errno));
                    exit(1);
                }
                
                 addrlen=sizeof(SM_addr);
                
                n= recvfrom(SM_Sock,return_msg,28500,0,(struct sockaddr*)&SM_addr, &addrlen);
                
                if(n==-1){//error
                    printf("error: %s\n", strerror(errno));
                    exit(1);
                }
                printf("%s",return_msg);

                
                break;
            case 4 : // ENCERRAR O PROGRAMA.
                printf("Programa Encerrado\n");
                close(SI_Sock);
                close(SM_Sock);
                exit(0);
                
                break;
            default : break;
        }
    }
    
    exit(0);
    
    
}
