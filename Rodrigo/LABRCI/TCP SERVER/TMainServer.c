//
//  main.c
//  TCP SERVER
//
//  Created by Rodrigo Lopes Do Ó Barbosa on 11/03/17.
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

int main(void) {
    
    int fd, addrlen, ret, wt, nw, newfd, n;
    struct sockaddr_in addr;
    char buffer[141], name[100], *ptr;
    struct hostent *h;
    struct in_addr *a;
    
    
    
    if (gethostname(name, 100)==-1) {           //GET HOST NAME
        printf("error: %s\n", strerror(errno));
    }
    
    
    if ((h = gethostbyname(name))==NULL) {      //GET HOST BY NAME
        printf("error: %s\n", strerror(errno));
        exit(1);
    }
    
    a=(struct in_addr*)h->h_addr_list[0];
    
    //PRINT SERVER NAME AND ADDRESS
    printf("Official Server Name: %s\n", h->h_name);
    printf("Server adress: %s (%08lX)\n", inet_ntoa(*a), (long unsigned int)ntohl(a->s_addr));
    
    
    
    fd=socket(AF_INET,SOCK_STREAM,0); //UDP SOCKET
    
    if(fd==-1){                                 //ERROR
        printf("error: %s\n", strerror(errno));
        exit(1);
    }
    
    memset((void*)&addr, (int)'\0', sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port=htons(9000);
    
    ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));  //BIND
    
    if(ret==-1){                                //ERROR
        printf("error: %s\n", strerror(errno));
        exit(1);
    }
    
    wt= listen(fd, 5);             //LISTEN
    
    if(wt==-1){                                //ERROR
        printf("error: %s\n", strerror(errno));
        exit(1);
    }
    
    while (1) {
        addrlen=sizeof(addr);
                                                                //CREAT NEW SOCKET
        newfd =  accept(fd, (struct sockaddr*)&addr, &addrlen);
        
        if (newfd==-1) {                                //ERROR
            printf("error: %s\n", strerror(errno));
            exit(1);
        }
        
                                                //READ NEW SOCKET
        while ((n=read(newfd, buffer, 141))!=0) {
            
                                                    //ERROR
            if (n==-1) {
                printf("error: %s\n", strerror(errno));
                exit(1);
            }
            
            
            ptr=&buffer[0];
            
            while (n>0) {
                
                nw = write(newfd, ptr, n);      // WRITE NEW SCOKET
                
                if (nw <=0) {                               //ERROR
                    printf("error: %s\n", strerror(errno));
                    exit(1);
                }
                
                            //NEXT CHARECTER
                n-=nw;
                ptr+=nw;
            }
            
        }
        //CLOSE NEW SOCKET
        close(newfd);
    }
    
    /*
     close(fd);
     exit(0);
     
     */
    
}
