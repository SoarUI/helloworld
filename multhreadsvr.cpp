#include<sys/types.h>
#include<unistd.h>
#include<sys/socket.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#define BACKLOG 5
#define MAXCHARSIZE 100

void process_client(int connectedfd,struct sockaddr_in client);
void  *start_routine(void*arg);
struct ARG
{
    int connfd;
    struct sockaddr_in client;
    ARG() {}
};
int main(int argc,char** argv){
    int listenfd,connectedfd;
    struct sockaddr_in serveraddr,clientaddr;
    int sin_size;
    int opt =SO_REUSEADDR;
    pthread_t tid;
    sin_size = sizeof(struct sockaddr_in);
    if(argc !=2)
    {
        fprintf(stderr,"%s<port>\n",argv[0]);
        exit(1);
    }
    if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("create listen socket error\n");
        exit(1);
    }
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof opt);
    memset(&serveraddr,0,sizeof serveraddr );
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
    serveraddr.sin_port=htons(atoi(argv[1]));
    if(bind(listenfd,(struct sockaddr*)&serveraddr,sizeof (serveraddr))==-1){
        perror("bind error\n");
        exit(1);
    }
    if(listen(listenfd,BACKLOG)==-1){
        perror("listen error\n");
        exit(1);
    }
    while(true)
    {
        if( (connectedfd = accept(listenfd,(struct sockaddr*)&clientaddr,(socklen_t*)&sin_size))==-1)
        {
            perror("Accept error\n");
            exit(1);
        }
        struct ARG* arg =(struct ARG*)malloc(sizeof(struct ARG));
        arg->connfd = connectedfd;
        memcpy((void*)&arg->client,&clientaddr,sizeof clientaddr);
        if(pthread_create(&tid,NULL,start_routine,(void*)arg))
        {
            perror("pthread_create error\n");
            exit(1);
        }

    }
    close(listenfd);
    return 0;
}
void process_client(int connectedfd, sockaddr_in client){
    char recvbuf[MAXCHARSIZE];
    char sendbuf[MAXCHARSIZE];
    char clientname[MAXCHARSIZE];
    int recvlen,i;
    printf("YOU get a connection:%s.\n",inet_ntoa(client.sin_addr));
    send(connectedfd,"Welcome to my server.\n",22,0);
    memset(recvbuf,0,MAXCHARSIZE);
    while ((recvlen =recv(connectedfd,recvbuf,MAXCHARSIZE,0))>0) {
        recvbuf[recvlen]=0;
        printf("Received :%s\n",recvbuf);
        for(i=0;i<recvlen;i++)
            sendbuf[i]=recvbuf[recvlen-i-1];
        sendbuf[recvlen]=0;
        send(connectedfd,sendbuf,strlen(sendbuf),0);
        memset(recvbuf,0,MAXCHARSIZE);

    }
    close(connectedfd);
}
void* start_routine(void *arg){
    struct ARG *info;
    info =(struct ARG*)arg;
    process_client(info->connfd,info->client);
    free(arg);
    pthread_exit(NULL);
}
