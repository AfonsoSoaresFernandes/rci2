//
//  main.c
//  LABRCI
//
//  Created by Rodrigo Lopes Do Ó Barbosa on 08/03/17.
//  Copyright (c) 2017 Rodrigo Lopes Do Ó Barbosa. All rights reserved.
//

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
extern int errno;


int main(void){
    
    
    // gethostname
    /*char buffer[128];
     
     if (gethostname(buffer, 128)==-1) {
     printf("error: %s\n", strerror(errno));
     }else{
     printf("host name: %s\n", buffer);
     exit (0);
     }*/
    
    //gethostbyname
    
   /* struct hostent *h;
    struct in_addr *a;
    
    if ((h = gethostbyname("macbook-de-rodrigo-lopes-do-o-barbosa-2.local"))==NULL) {
        printf("error: %s\n", strerror(errno));
        exit(1);
    }
    
    printf("official host name: %s\n", h->h_name);
    
    a=(struct in_addr*)h->h_addr_list[0];
    
    printf("internet adress: %s (%08lX)\n", inet_ntoa(*a), (long unsigned int)ntohl(a->s_addr));
    exit(0);*/
    
    //UDP Conection
    
     int fd, n, addrlen;
     char buffer[141];
     struct sockaddr_in addr;
     struct hostent *h;
     struct in_addr *a;
     
     
      struct in_addr iaddr;                    //   CENA DO PROF!!!
     
     inet_aton("192.168.1.97",&iaddr);
     
     if ((h = gethostbyname("macbook-de-rodrigo-lopes-do-o-barbosa-2.local"))==NULL) {    //GET HOST NAME
     printf("error: %s\n", strerror(errno));
     exit(1);
     }
    
     a=(struct in_addr*)h->h_addr_list[0];
     
    
     
     fd=socket(AF_INET,SOCK_DGRAM,0); //UDP SOCKET
     
     if(fd==-1){
     printf("error: %s\n", strerror(errno));
     exit(1);
     }
     
     memset((void*)&addr, (int)'\0', sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr= iaddr;
     addr.sin_port=htons(9000);
     
     n=sendto(fd,"PUBLISH OLA",11,0,(struct sockaddr*)&addr, sizeof(addr));
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
     
     write(1,"Echo: ",6);//stdout
     write(1,buffer,n);
     close(fd);
     
     h = gethostbyaddr(&addr.sin_addr,sizeof(addr.sin_addr),AF_INET);  //GET HOST BY ADDR
     
     if (h==NULL) {
     printf("Sent by: [%s:%hu]\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

     }else{
     printf("Sent by: [%s:%hu]\n", h->h_name, ntohs(addr.sin_port));
     }
     exit(0);
     

}
