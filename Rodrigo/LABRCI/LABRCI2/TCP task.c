//
//  main.c
//  LABRCI2
//
//  Created by Rodrigo Lopes Do Ó Barbosa on 09/03/17.
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
# include <signal.h>
extern int errno;


int main(void){
    
    int fd, n, nleft, nbytes, nwritten, nread;
    struct sockaddr_in addr;
    char *ptr, buffer[141];
    struct hostent *h;
    struct in_addr *a;
    void (*old_handler)(int);   //Interrupt Handler
    
    
    //alterado
    
    struct in_addr iaddr;
    inet_aton("192.168.1.97",&iaddr);
    
   /* if ((h = gethostbyname("afonso-VirtualBox"))==NULL) {    //GET HOST NAME
        printf("error: %s\n", strerror(errno));
        exit(1);
    }
    
    a=(struct in_addr*)h->h_addr_list[0];*/
    
    fd = socket(AF_INET, SOCK_STREAM, 0); // TCP SOCKET
    
    if(fd==-1){                                 //ERROR
        printf("error: %s\n", strerror(errno));
        exit(1);
    }
    
    memset((void*)&addr, (int)'\0', sizeof(addr)); //ADDRESS INIT
    addr.sin_family=AF_INET;
    addr.sin_addr= iaddr;
    addr.sin_port=htons(9000);

    n = connect(fd, (struct sockaddr*)&addr, sizeof(addr)); // Conecting to Server
    
    if(n==-1){                                      //ERROR
        printf("error: %s\n", strerror(errno));
        exit(1);
    }
    
    stpcpy(buffer, "Cheguei!\n");
    ptr = buffer;
    nbytes = 9;

    
    nleft = nbytes;                             //WRITE MESSAGE
    while (nleft>0) {
        
        if ((old_handler=signal(SIGPIPE, SIG_IGN))==SIG_ERR) {  //ERROR IF CONECION LOST
            printf("error: %s\n", strerror(errno));
            exit(1);
        }
        
        
        nwritten= write(fd, ptr, nleft);
        
        if (nwritten<=0) {                          //ERROR
            printf("error: %s\n", strerror(errno));
            exit(1);
        }
        nleft-=nwritten;
        ptr+=nwritten;
    }
    
    nleft = nbytes;
    ptr = buffer;
    while (nleft>0) {                           //READ MESSAGE
        nread = read(fd, ptr, nleft);
        
        if (nread==-1) {                               //ERROR
            printf("error: %s\n", strerror(errno));
            exit(1);
        }else if(nread==0){                             //CLOSED BY PEER
            break;
        }
        
        nleft-=nread;
        ptr+=nread;
    }
    nread=nbytes-nleft;
    close(fd);
    
    write(1, "Echo: ", 6);
    write(1, buffer, nread);
    
    
    h = gethostbyaddr(&addr.sin_addr,sizeof(addr.sin_addr),AF_INET);  //GET HOST BY ADDR
    
    if (h==NULL) {
        printf("Sent by: [%s:%hu]\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        
    }else{
        printf("Sent by: [%s:%hu]\n", h->h_name, ntohs(addr.sin_port));
    }
    exit(0);
}