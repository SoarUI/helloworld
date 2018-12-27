#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<stdlib.h>
#define BUFLEN 255
int main(int argc,char **argv){
    struct sockaddr_in localaddr;
    int clifd,n;
    char msg[BUFLEN+1];
    if(argc != 2){
        printf("usage:%s<port>\n",argv[1]);
        exit(0);
    }
    clifd =socket(AF_INET,SOCK_DGRAM,0);
    if(clifd <0){
        fprintf(stderr,"creating socket error\n");
        exit(0);
    }
    memset(&localaddr,0,sizeof localaddr);
    localaddr.sin_family=AF_INET;
    localaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    localaddr.sin_port=htons(atoi(argv[1]));
    int opt = SO_REUSEADDR;
    setsockopt(clifd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof (int));
    if(bind(clifd,(struct sockaddr*)&localaddr,sizeof(struct sockaddr_in))<0){
        fprintf(stderr,"bind error\n");
        exit(0);
    }
    n =read(clifd,msg,BUFLEN);
    if(n == -1){
        fprintf(stderr,"read error\n");
        exit(0);
    }
    msg[n]=0;
    printf("%s\n",msg);
    return 0;
}
