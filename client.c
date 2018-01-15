#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MaxMesg 243
#define MaxName  12
#define PORT "8080"

void *Write (void *fd) 
{
    int sock_fd = *(int *)fd;
    char Mesg[MaxMesg], Send[MaxMesg + MaxName], Name[MaxName];
    bzero(Mesg, sizeof(Mesg));
    bzero(Send, sizeof(Send));
    printf("Enter your name: ");
    scanf("%s\n", Name);
    while(1) 
	{   
        fgets(Mesg, 100, stdin);
        sprintf(Send, "%s: %s", Name, Mesg);
        send(sock_fd, Send, strlen(Send)+1, 0);
	}
} 

int main(int argc,char **argv)
{
    if (argc < 2) {
        printf("Server IP Address needed\n");
        return 0; 
    }
    int sock_fd;
    char Rcvd[MaxMesg];
    char portno[6] = PORT;
    struct sockaddr_in servaddr;
    pthread_t tid;

    if (argc > 2) { strcpy(portno, argv[2]); } 
    sock_fd = socket(AF_INET,SOCK_STREAM, 0);
    bzero(&servaddr, sizeof servaddr);
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(atoi(portno));
    inet_pton(AF_INET, argv[1], &(servaddr.sin_addr));
    if (connect(sock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == 0) {
        pthread_create(&tid, NULL, Write, &sock_fd);
        bzero( Rcvd, 100);
        while(recv(sock_fd, Rcvd, 100, 0))
        {
            printf("%s", Rcvd);
        }
        printf("Server Quit Running\n");
        pthread_cancel(tid);
    }
    else printf("Server Not Running\n");
}

