#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define MAXLINE 1024
#define TRUE 1

int main(int argc, char **argv) {
    int sockfd, fd, n, m;
    char line[MAXLINE + 1];
    char linein[MAXLINE + 1];
    char buffer[MAXLINE + 1];
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

    int flag[10] = {0};
    // for(int i=0; i<atoi(argv[1])-1; i++){
    //     pid_t usrpid = fork();
    // }

    while(TRUE) {
        // printf("%d\n",getpid());
        printf("> Waiting clients ...\r\n");
        socklen_t clilen = sizeof(struct sockaddr);
        fd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen);
        if(fd == -1) { 
            perror("accept error");
        }
        printf("> Accepted.\r\n");

        pid_t fpid1 = fork();
        int status1, status2;
        if (fpid1==0){
            while((n = read(fd, linein, MAXLINE)) > 0) { 
                linein[n] = 0;
                if(fputs(linein, stdout) == EOF)
                    perror("fputs error");
                // strcpy(buffer, linein);
                // for(int i=0;i<10;i++)
                //     flag[i]=1;
                // flag[getppid()%10]=0;
                // for(int i=0;i<10;i++)
                //     printf("%d",flag[i]);
                // printf("\n");

                char endstr[] = "END";
                char linestr[MAXLINE + 1];
                strcpy(linestr, linein);
                linestr[n-1] = 0;
                if(strcmp(endstr, linestr)==0)
                    break;
            }
            exit(0);
        }        
        else{
            pid_t fpid2 = fork();
            if(fpid2==0){
                while(fgets(line, MAXLINE, stdin) != NULL) { 
                    send(fd, line, strlen(line), 0);
                    char endstr[] = "END";
                    char linestr[MAXLINE + 1];
                    strcpy(linestr, line);
                    linestr[strlen(line)-1] = 0;
                    if(strcmp(endstr, linestr)==0)
                        break;               
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
        close(fd);
    }
    if(n < 0) 
        perror("read error");
    exit(0);
}
