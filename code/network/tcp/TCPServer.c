#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 1024
#define TRUE 1

int main(int argc, char **argv) {
    int sockfd, fd, n, m;
    char line[MAXLINE + 1];
    struct sockaddr_in6 servaddr, cliaddr;
    time_t t0 = time(NULL);
    printf("time #: %ld\n", t0);
    fputs(ctime(&t0), stdout);

    if((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
        perror("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_port = htons(20000);
    servaddr.sin6_addr = in6addr_any;

    if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
        perror("bind error");

    if(listen(sockfd, 5) == -1)
        perror("listen error");

    while(TRUE) {
        printf("> Waiting clients ...\r\n");
        socklen_t clilen = sizeof(struct sockaddr);
        fd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen);
        if(fd == -1) { 
            perror("accept error");
        }
        printf("> Accepted.\r\n");
        while((n = read(fd, line, MAXLINE)) > 0) { 
            line[n] = 0;
            if(fputs(line, stdout) == EOF)
                perror("fputs error");
        }
        close(fd);
    }
    if(n < 0) 
        perror("read error");
    exit(0);
}
