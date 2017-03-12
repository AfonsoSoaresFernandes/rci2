//task 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
extern int errno;

/*int main(void)
{
char buffer[128];
if(gethostname(buffer,128)==-1)printf("error: %s\n", strerror(errno));   //vai buscar o nome do host
else printf("host name: %s\n",buffer);
exit(0);
}
*/

//task 2
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*int main(void){
  struct hostent *h;
  struct in_addr *a;
  if((h=gethostbyname("tejo.ist.utl.pt"))==NULL){      //vai buscar a informação do server com aquele nome
    printf("servidor em baixo \n\n");
    exit(1);
  }//error
  printf("official host name: %s\n",h->h_name);
  a=(struct in_addr*)h->h_addr_list[0];     //vai buscar o internet adress ao struct hostent* h que vem numa posição do vector de strings (devido a vir em hexadecimal)
  printf("internet address: %s (%08lX)\n",inet_ntoa(*a),(long unsigned int)ntohl(a->s_addr));   // inet_ntoa() vai buscar o nome a struct in_addr *a
  exit(0);
}*/


//task 3 UDP, socket and sendto
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

/*int main(void){
  int fd, n;
  struct sockaddr_in addr;
  struct hostent *h;
  struct in_addr *a;

  if((h=gethostbyname("tejo"))==NULL){
    printf("error: %s\n", strerror(errno));
    exit(2);
  }
  a=(struct in_addr*)h->h_addr_list[0];

  fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
  if(fd==-1)exit(1);//error
  memset((void*)&addr,(int)'\0',sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr=*a;
  addr.sin_port=htons(58000);
  n=sendto(fd,"Hello!\n",7,0,(struct sockaddr*)&addr,sizeof(addr));
  if(n==-1){
  printf("error: %s\n", strerror(errno));
  exit(1);//error
  }

  close(fd);
  exit(0);
}
*/

//task 4 UDP and recvfrom
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

/*int main(void){
  int fd, n, addrlen;
  struct sockaddr_in addr;
  char buffer[128];
  struct hostent *h;
  struct in_addr *a;

  if((h=gethostbyname("tejo.ist.utl.pt"))==NULL){
    printf("1 error: %s\n", strerror(errno));
    exit(2);
  }
  a=(struct in_addr*)h->h_addr_list[0];

  fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
  if(fd==-1)exit(1);//error
  memset((void*)&addr,(int)'\0',sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr=*a;
  addr.sin_port=htons(58000);


  n=sendto(fd,"hello!\n",7,0,(struct sockaddr*)&addr,sizeof(addr));
  if(n==-1){
    printf("2 error: %s\n", strerror(errno));
    exit(1);//error
  }

  addrlen=sizeof(addr);
  n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
  if(n==-1)exit(1);//error
  write(1,"echo: ",6);//stdout
  write(1,buffer,n);

  close(fd);
  exit(0);
}*/

//task 5  gethostbyaddr
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>

/*int main(void){
  int fd, n, addrlen;
  struct sockaddr_in addr;
  char buffer[128];
  struct hostent *h;
  struct in_addr *a;

  if((h=gethostbyname("tejo"))==NULL){
    printf("1 error: %s\n", strerror(errno));
    exit(2);
  }
  a=(struct in_addr*)h->h_addr_list[0];

  fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
  if(fd==-1)exit(1);//error
  memset((void*)&addr,(int)'\0',sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr=*a;
  addr.sin_port=htons(58000);


  n=sendto(fd,"hello!\n",7,0,(struct sockaddr*)&addr,sizeof(addr));
  if(n==-1){
    printf("2 error: %s\n", strerror(errno));
    exit(1);//error
  }

  addrlen=sizeof(addr);
  n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
  if(n==-1)exit(1);//error
  write(1,"echo: ",6);//stdout
  write(1,buffer,n);

  h=gethostbyaddr(&addr.sin_addr,sizeof(addr.sin_addr),AF_INET);// esta parte é toda um pouco redundante porque temos o addr desde o inicio em que fazemos get host

  if(h==NULL)printf("sent by [%s:%hu]\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
  else printf("sent by [%s:%hu]\n",h->h_name,ntohs(addr.sin_port));

  close(fd);
  exit(0);
}
*/

//task 6 TCP, socket and connect


#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

/*int main(void){
  int fd, n, addrlen;
  struct sockaddr_in addr;
  char buffer[128];
  struct hostent *h;
  struct in_addr *a;

  if((h=gethostbyname("tejo.ist.utl.pt"))==NULL){
    printf("1 error: %s\n", strerror(errno));
    exit(2);
  }
  a=(struct in_addr*)h->h_addr_list[0];

  fd=socket(AF_INET,SOCK_STREAM,0);//TCP socket
  if(fd==-1){
    printf("Erro na socket\n");
    exit(1);
  }//error
  memset((void*)&addr,(int)'\0',sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr=*a;
  addr.sin_port=htons(58000);

  n=connect(fd,(struct sockaddr*)&addr,sizeof(addr));//Estabelecer a ligação TCP com o server
  if(n==-1){
    printf("\n");
    exit(1);
  }

  n=sendto(fd,"hello!\n",7,0,(struct sockaddr*)&addr,sizeof(addr));
  if(n==-1){
    printf("2 error: %s\n", strerror(errno));
    exit(1);//error
  }

  addrlen=sizeof(addr);
  n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
  if(n==-1){
    printf("erro no receive from\n");
    exit(1);
  }//error
  write(1,"echo: ",6);//stdout
  write(1,buffer,n);

  h=gethostbyaddr(&addr.sin_addr,sizeof(addr.sin_addr),AF_INET);// esta parte é toda um pouco redundante porque temos o addr desde o inicio em que fazemos get host

  if(h==NULL)printf("sent by [%s:%hu]\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
  else printf("sent by [%s:%hu]\n",h->h_name,ntohs(addr.sin_port));

  close(fd);
  exit(0);
}
*/

//task 7  TCP, write and read

#include <unistd.h>
#include <string.h>

/*int main(void){
  int fd, n, addrlen,nbytes, nleft, nwritten, nread;
  struct sockaddr_in addr;
  char *ptr, buffer[128];
  struct hostent *h;
  struct in_addr *a;



  if((h=gethostbyname("tejo.ist.utl.pt"))==NULL){
    printf("1 error: %s\n", strerror(errno));
    exit(2);
  }
  a=(struct in_addr*)h->h_addr_list[0];

  fd=socket(AF_INET,SOCK_STREAM,0);//TCP socket
  if(fd==-1){
    printf("Erro na socket\n");
    exit(1);
  }//error
  memset((void*)&addr,(int)'\0',sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr=*a;
  addr.sin_port=htons(58000);

  n=connect(fd,(struct sockaddr*)&addr,sizeof(addr));//Estabelecer a ligação TCP com o server
  if(n==-1){
    printf("cagalhao\n");
    exit(1);
  }

  ptr=strcpy(buffer,"Hello!\n");
  nbytes=7;

  printf("debugger %s\n", ptr );

  nleft=nbytes;
  while(nleft>0){
    nwritten=write(fd,ptr,nleft);
    if(nwritten<=0){
      printf("ciclo while é merda");
      exit(1);
    }//error
    nleft-=nwritten;
    ptr+=nwritten;}

  nleft=nbytes;
  ptr=buffer;

  while(nleft>0){
    nread=read(fd,ptr,nleft);
    if(nread==-1){
      printf("ciclo while é merda");
      exit(1);
    }//error
    else if(nread==0){
      printf("Passou pelo else a ler");
      break;//closed by peer
    }
    nleft-=nread;
    ptr+=nread;}

  nread=nbytes-nleft;

  write(1,"echo: ",6);//stdout
  write(1,buffer,nread);

  printf("\n\nit worked\n\n");

  h=gethostbyaddr(&addr.sin_addr,sizeof(addr.sin_addr),AF_INET);// esta parte é toda um pouco redundante porque temos o addr desde o inicio em que fazemos get host

  if(h==NULL)printf("sent by [%s:%hu]\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
  else printf("sent by [%s:%hu]\n",h->h_name,ntohs(addr.sin_port));

  close(fd);
  exit(0);
}*/

//task 8 Lidar com o SIGPIPE quando a ligação se perde durante o ciclo while do write
#include <signal.h>

/*int main(void){
  int fd, n, addrlen,nbytes, nleft, nwritten, nread;
  struct sockaddr_in addr;
  char *ptr, buffer[128];
  struct hostent *h;
  struct in_addr *a;

  void (*old_handler)(int);

  if((h=gethostbyname("tejo.ist.utl.pt"))==NULL){
    printf("1 error: %s\n", strerror(errno));
    exit(2);
  }
  a=(struct in_addr*)h->h_addr_list[0];

  fd=socket(AF_INET,SOCK_STREAM,0);//TCP socket
  if(fd==-1){
    printf("Erro na socket\n");
    exit(1);
  }//error
  memset((void*)&addr,(int)'\0',sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr=*a;
  addr.sin_port=htons(58000);

  n=connect(fd,(struct sockaddr*)&addr,sizeof(addr));//Estabelecer a ligação TCP com o server
  if(n==-1){
    printf("cagalhao\n");
    exit(1);
  }

  ptr=strcpy(buffer,"Hello!\n");
  nbytes=7;

  nleft=nbytes;

  while(nleft>0){
    nwritten=write(fd,ptr,nleft);
    if(nwritten<=0){
      printf("ciclo while é merda");
      exit(1);
    }//error
    nleft-=nwritten;
    ptr+=nwritten;}

  nleft=nbytes; ptr=buffer;

  while(nleft>0){
    nread=read(fd,ptr,nleft);
    if(nread==-1){
      printf("ciclo while é merda");
      exit(1);
    }//error
    else if(nread==0){
      printf("Passou pelo else a ler");
      break;//closed by peer
    }
    nleft-=nread;
    ptr+=nread;}

  nread=nbytes-nleft;

  write(1,"echo: ",6);//stdout
  write(1,buffer,nread);
  printf("\n\nit worked\n\n");

  h=gethostbyaddr(&addr.sin_addr,sizeof(addr.sin_addr),AF_INET);// esta parte é toda um pouco redundante porque temos o addr desde o inicio em que fazemos get host

  if(h==NULL)printf("sent by [%s:%hu]\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
  else printf("sent by [%s:%hu]\n",h->h_name,ntohs(addr.sin_port));

  close(fd);
  exit(0);
}*/


//task 9 criar um servidor eccho UDP e bind

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

/*int main(void){
  int fd, addrlen, ret, nread;
  struct sockaddr_in addr;
  char buffer[128];

  if((fd=socket(AF_INET,SOCK_DGRAM,0))==-1)exit(1);//error

  memset((void*)&addr,(int)'\0',sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=htonl(INADDR_ANY);
  addr.sin_port=htons(9000);

  ret=bind(fd,(struct sockaddr*)&addr,sizeof(addr));

  if(ret==-1)exit(1);//error

  printf("O nome do server eccho é: %s\n",inet_ntoa(addr.sin_addr));

  while(1){
    addrlen=sizeof(addr);
    nread=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
    if(nread==-1)exit(1);//error
    ret=sendto(fd,buffer,nread,0,(struct sockaddr*)&addr,addrlen);
    if(ret==-1)exit(1);//error
  }
close(fd);
exit(0);
}*/


//task 10 TCP server, bind, listen and accept

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main(void){

struct hostent *g;
int fd, addrlen, newfd, r;
struct sockaddr_in addr;
int n, nw;
char *ptr, buffer[128];
struct in_addr *a;



if((fd=socket(AF_INET,SOCK_STREAM,0))==-1)exit(1);//error

memset((void*)&addr,(int)'\0',sizeof(addr));
addr.sin_family=AF_INET;
addr.sin_addr.s_addr=htonl(INADDR_ANY);
addr.sin_port=htons(9000);

if(bind(fd,(struct sockaddr*)&addr,sizeof(addr))==-1)
 exit(1);//error

if(listen(fd,5)==-1)exit(1);//error

if(gethostname(buffer,128)==-1)printf("error: %s\n", strerror(errno));
printf("Host name: %s\n", buffer);

if((g=gethostbyname(buffer))==NULL)printf("nao encontrou o host pelo nome");
else{
  a=(struct in_addr*)g->h_addr_list[0];
  printf("internet address: %s (%08lX)\n",inet_ntoa(*a),(long unsigned int)ntohl(a->s_addr));
}
printf("o address: %u\n", a->s_addr);
r=0;
ptr=&buffer[0];
while(1){
  memset(buffer, 0, strlen(buffer)); // FUNÇÃO QUE LIMPA STRINGS
  addrlen=sizeof(addr);
  if((newfd=accept(fd,(struct sockaddr*)&addr,&addrlen))==-1)exit(1);//error
  while((n=read(newfd,buffer,140))!=0){if(n==-1)exit(1);//error
    printf("recebido no server %s\n", buffer);
    ptr=&buffer[0];
    while(n>0){if((nw=write(newfd,ptr,n))<=0)exit(1);//error
      n-=nw; ptr+=nw;}
    }
  printf("\n");
  printf("li e devolvi: %d mensagens\n", ++r);
  close(newfd);
}
close(fd); exit(0);
}
