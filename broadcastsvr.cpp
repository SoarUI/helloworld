#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<sys/time.h>
#include<time.h>
#define BUFLEN 255

void getcurtime(char* curtime){
    time_t tm;
    time(&tm);
    /*z*/
    snprintf(curtime,BUFLEN,"%s\n",ctime(&tm));
}
int main(int argc,char **argv){
    struct sockaddr_in peeraddr;
    int sockfd,on =1;
    int num,i;
    char msg[BUFLEN+1];
    if(argc !=3){
        printf("usage:%s<ip  address><port>\n",argv[0]);
        exit(0);

    }
    sockfd =socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0){
        fprintf(stderr,"socket creating error in broadcast.cpp\n");
        exit(0);
    }
    setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&on,sizeof(int));
    memset(&peeraddr,0,sizeof peeraddr);
peeraddr.sin_family =AF_INET;
if(inet_pton(AF_INET,argv[1],&peeraddr.sin_addr)<=0){
    printf("Wrong dest IP address\n");
    exit(0);
}
//peeraddr.sin_addr.s_addr=htonl(INADDR_BROADCAST);
peeraddr.sin_port=htons(atoi(argv[2]));
for(;;){
    getcurtime(msg);
    int a;
    a =sendto(sockfd,msg,strlen(msg),0,(struct sockaddr*)&peeraddr,sizeof(struct sockaddr_in));
    printf("%d\n",a);
    fflush(stdout);
    sleep(3);
}
return 0;
}
