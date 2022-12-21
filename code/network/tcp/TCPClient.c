#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define MAXLINE 1024
#define TRUE 1

int main(int argc, char **argv) {
    int sockfd, n, m;
    char line[MAXLINE + 1];
    char linein[MAXLINE + 1];
    struct sockaddr_in6 servaddr;
    time_t t0 = time(NULL);
    printf("time #: %ld\n", t0);
    fputs(ctime(&t0), stdout);

    if(argc != 2)
        perror("usage: a.out <IPaddress>");

    if((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
        perror("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_port = htons(20000);

    if(inet_pton (AF_INET6, argv[1], &servaddr.sin6_addr) <= 0)
        perror("inet_pton error");

    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        perror("connect error");

    pid_t fpid1 = fork();
    int status1, status2;
    if(fpid1==0){
        while((n = read(sockfd, linein, MAXLINE)) > 0) { 
            linein[n] = 0;
            if(fputs(linein, stdout) == EOF)
                perror("fputs error");
            char endstr[] = "END";
            char linestr[MAXLINE + 1];
            strcpy(linestr, linein);
            linestr[n-1] = 0;
            if(strcmp(endstr, linestr)==0){
                break; 
            }     
        }      
        exit(0);
    }
    else{
        pid_t fpid2 = fork();
        if(fpid2==0){
            while(fgets(line, MAXLINE, stdin) != NULL) { 
                send(sockfd, line, strlen(line), 0);
                char endstr[] = "END";
                char linestr[MAXLINE + 1];
                strcpy(linestr, line);
                linestr[strlen(line)-1] = 0;
                if(strcmp(endstr, linestr)==0){
                    break;
                }                
            }
            exit(0);
        }
        else{
            while(1){
                if(waitpid(fpid1, &status1, WNOHANG)||waitpid(fpid2, &status2, WNOHANG)){
                    kill(fpid1, SIGTERM);
                    kill(fpid2, SIGTERM);
                    break;
                }       
            }
        }
    }
    
    close(sockfd);
    exit(0);
}