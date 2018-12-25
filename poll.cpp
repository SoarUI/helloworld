#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <errno.h>
#include<poll.h>

#define MAX_EVENTS 100
#define PORT 8080

//设置socket连接为非阻塞模式
void setnonblocking(int sockfd) {
    int opts;

    opts = fcntl(sockfd, F_GETFL);
    if(opts < 0) {
        perror("fcntl(F_GETFL)\n");
        exit(1);
    }
    opts = (opts | O_NONBLOCK);
    if(fcntl(sockfd, F_SETFL, opts) < 0) {
        perror("fcntl(F_SETFL)\n");
        exit(1);
    }
}

int main(){
    struct pollfd ev, events[MAX_EVENTS];
    int addrlen, listenfd, conn_sock, nfds, epfd, fd, i, nread, n;
    struct sockaddr_in local, remote;
    char buf[BUFSIZ];

    //创建listen socket
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("sockfd\n");
        exit(1);
    }
    setnonblocking(listenfd);
    bzero(&local, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);;
    local.sin_port = htons(PORT);
    if( bind(listenfd, (struct sockaddr *) &local, sizeof(local)) < 0) {
        perror("bind\n");
        exit(1);
    }
    listen(listenfd, 20);

memset(events,-1,sizeof events );
    events[0].events = POLLIN;
    events[0].fd = listenfd;


    for (;;) {
        nfds = poll(events, MAX_EVENTS, -1);
        if (nfds <0) {
            perror("epoll_pwait");
            exit(EXIT_FAILURE);
        }
    if(nfds==0){
        continue;}
        for (i = 0; i < MAX_EVENTS; ++i) {
            fd = events[i].fd;
            if(fd<0)
                continue;
            if (fd == listenfd && (events[i].revents & POLLIN) ){
                while ((conn_sock = accept(listenfd,(struct sockaddr *) &remote,
                                (socklen_t *)&addrlen)) > 0) {
                    setnonblocking(conn_sock);

                    for(int j=0;j<MAX_EVENTS;j++){
                        if(events[j].fd<0){
                            events[j].events = POLLIN ;
                            events[j].fd = conn_sock;
                            break;
                        }
                    }//for

                }
                if (conn_sock == -1) {
                    if (errno != EAGAIN && errno != ECONNABORTED
                            && errno != EPROTO && errno != EINTR)
                        perror("accept");
                }
                continue;
            }
            if (events[i].revents & POLLIN) {
                n = 0;
                while ((nread = read(fd, buf + n, BUFSIZ-1)) > 0) {
                    n += nread;
                }
                if (nread == -1 && errno != EAGAIN) {
                    perror("read error");
                }
                //close socket
                if(n==0){
                    events[i].fd=-1;
                }
                events[i].events |= POLLOUT;
                 continue;
                }
            if (events[i].revents & POLLOUT) {

                sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nHello World", 11);
                int nwrite, data_size = strlen(buf);
                n = data_size;
                while (n > 0) {
                    printf("n:%d i:%d\n",n,i);
                    nwrite = write(fd, buf + data_size - n, n);
                    if (nwrite < n) {
                        if (nwrite == -1 && errno != EAGAIN) {
                            perror("write error");
                        }
                        break;
                    }
                     printf("nw:%d i:%d\n",nwrite,i);
                    n -= nwrite;
                     events[i].events &= ~POLLOUT;
                }

            }
        }
  }
    return 0;
}
