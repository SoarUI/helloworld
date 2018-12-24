#include<unistd.h>
#include<sys/select.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<netinet/in.h>
#include<sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#define MAXCLIENT 100
#include<string.h>
#include<fcntl.h>
void convnonblock(int fd){
int flags =0;
if((flags= fcntl(fd,F_GETFL))<0){
perror("fcntl error!\n");
return;}
flags |=O_NONBLOCK;
if(fcntl(fd,F_SETFL,flags) ){
perror("fcntl(F_SETFL)\n");
exit(1);
}
}
int main()
{
int serverfd,client[MAXCLIENT];
socklen_t serverlen,clientlen;
struct sockaddr_in serveraddr;
struct sockaddr_in clientaddr;
int result;
int maxfd=0;
fd_set rds,testrds;
serverfd =socket(AF_INET,SOCK_STREAM,0);
serveraddr.sin_family=AF_INET;
serveraddr.sin_port=htons(8080);
serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
serverlen =sizeof(serveraddr);
bind(serverfd,(struct sockaddr*)&serveraddr,serverlen);
listen(serverfd,100);
FD_ZERO(&rds);
FD_SET(serverfd,&rds);
memset(client,-1,sizeof(client));
char buf[BUFSIZ];
while(true){
printf("one loop begin\n");
FD_ZERO(&rds);
FD_SET(serverfd,&rds);
maxfd =serverfd;
for(int i =0;i < MAXCLIENT;i++)
{
	if(client[i]>0){
	FD_SET(client[i],&rds);
	if(maxfd < client[i])
		maxfd =client[i];
}
}
result = select(maxfd+1,&rds,NULL,NULL,NULL);
if(result <0 ){
printf("sock error!\n");
}
for(int i =0;i<MAXCLIENT;i++){
if(client[i]>0 &&FD_ISSET(client[i],&rds)){
	int n,nread;
n=0;
while((nread =recv(client[i],buf+n,BUFSIZ-1,0))>0){
n+=nread;
}
if((nread==-1&& errno !=EAGAIN)
||n==0){
perror("read error!\n");
close(client[i]);
client[i] =-1;
}
if(n>0)
printf("recv error:%s\n",buf);
}
}
 if(FD_ISSET(serverfd,&rds)){
 int clisock =accept(serverfd,NULL,NULL);
if(clisock == -1)
{
printf("accept error!\n");
}
convnonblock(clisock);

printf("client accept:%d\n",clisock);
for(int i =0;i<MAXCLIENT;i++){
if(client[i]<0){
client[i]=clisock;
if(maxfd < clisock)
maxfd =clisock;
break;
}

}
}
}
printf("exit program\n");
return 0;
}
