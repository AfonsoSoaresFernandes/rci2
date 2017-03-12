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
 SI_port            Porto de Servidor de Identidades
 SI_ip              IP do Servidor de Identidades
 SI_addr            Address de Servidor de Identidades
 
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
extern int errno;

int main(int argc, /*const*/ char * argv[]) {
    
    int SI_port=59000;
    int count, limit;
    char SI_ip [16];
    int fd, n, addrlen;
    char buffer[141];
    struct sockaddr_in SI_addr;
    struct hostent *h;
    struct in_addr iSI_addr;
    
    strcpy(SI_ip, "193.136.138.142");
    
                                                    //TRATAMENTO DE DADOS DE ENTRADA
     if (argc == 0 || argc == 3 || argc == 5 ) {
         limit=argc-2;
        for (count = 1; (count <= limit); count= (count + 2)) {
            if ((strcmp(argv[count], "-i"))==0) {
                strcpy(SI_ip, argv[count+1]);
            }else if ((strcmp(argv[count], "-p"))==0){
                SI_port = atoi(argv[count+1]);
            }else if(((strcmp(argv[count], "-i"))!=0 ) || ((strcmp(argv[count], "-p"))==0)){
                printf("Incorrect Appliction Calling\n ");
             exit(1);
            }

        }
     }else{
         printf("Incorrect Appliction Calling\n ");
         exit(1);
     }
    
    
    
    //Conexção UDP a executar.
   /*
                                //IP CONVERSION
    inet_aton(SI_ip,&iSI_addr);
    
    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP SOCKET
    
    if(fd==-1){                                 //ERROR
        printf("error: %s\n", strerror(errno));
        exit(1);
    }
    
    memset((void*)&SI_addr, (int)'\0', sizeof(SI_addr));
    SI_addr.sin_family=AF_INET;
    SI_addr.sin_addr= iSI_addr;
    SI_addr.sin_port=htons(SI_port);

    n=sendto(fd,"Cheguei!\n",9,0,(struct sockaddr*)&addr, sizeof(addr));
    if(n==-1){
        printf("error: %s\n", strerror(errno));
        exit(1);
    }
    
    addrlen=sizeof(addr);
    
    n= recvfrom(fd,buffer,141,0,(struct sockaddr*)&addr, &addrlen);
    
    if(n==-1){//error
        printf("error: %s\n", strerror(errno));
        exit(1);
    }
*/
    
    exit(0);
    
    
}
