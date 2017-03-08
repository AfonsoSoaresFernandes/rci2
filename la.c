//task 1
/*#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
extern int errno;
int main(void){

  char buffer[128];
  if(gethostname(buffer,128)==-1)
  printf(�error: %s\n",strerror(errno));
  else printf(�host name: %s\n",buffer);
  exit(0);
}
*/

//task 3
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
int main(void)
{
int fd, n;
  struct sockaddr_in addr;
  fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
  if(fd==-1)exit(1);//error
  memset((void*)&addr,(int)'\0',sizeof(addr));
  addr.sin_family=AF_INET;
  if((addr.sin_addr=gethostbyname("tejo"))==NULL)exit(-1);
  addr.sin_port=htons(8000);
  n=sendto(fd,"Hello!\n",7,0,(struct sockaddr*)&addr,sizeof(addr));
  if(n==-1)exit(1);//error
}
