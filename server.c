#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MaxMesg 256
#define MaxConn  10
#define PORT "8080"

/*Thread Arguments Structure*/
struct thread_args { 
        int *fd; 
        int cnt;
    };
/*Thread Function to Read Write*/
void *ReadWrite (void *Args) 
{
    struct thread_args *args = Args;
	int *fd = args->fd;
    int cnt = args->cnt;
	char Rcvd[MaxMesg];
	bzero(Rcvd, MaxMesg);
    int i;
	while(recv(fd[cnt], Rcvd, MaxMesg, 0)) 
	{   
        for (i=0; i<=args->cnt; i++){
            if((i != cnt) && (fd[i] != -1)) send(fd[i], Rcvd, strlen(Rcvd)+1, 0);}
	}
    printf("fd %d quit\n", fd[cnt]);
    fd[cnt] = -1;
	pthread_exit(NULL);
	return 0;
}
/*Main Program*/
int main(int argc, char *argv[])
{
    char portno[6] = PORT;
    int sock_fd; 
    int cnt = 0;
    int conn_fd[MaxConn] = {0};
    struct sockaddr_in servaddr, clntaddr;
    int sock_len = sizeof(struct sockaddr_in);    
    pthread_t tid;
  /*get IP address*/
    FILE *in = NULL;  
    char IP[16];
    in = popen("ip -o -f inet address show scope global|grep -Poz '(?<=inet ).*(?=/)'", "r");
    fgets(IP, 16, in);
  /*Get Port Number if Supplied*/
    if (argc > 1) { strcpy(portno, argv[1]); }
  /*Fill servaddr Structure*/
    bzero( &servaddr, sizeof(servaddr));
    bzero( &clntaddr, sizeof(clntaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(portno));
    inet_pton(AF_INET, IP, &(servaddr.sin_addr));
  /*create socket, bind and listen*/
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    bind(sock_fd, (struct sockaddr *) &servaddr, sock_len);
    listen(sock_fd, 10);
    printf("Started Server at %s with port %s\n", IP, portno);
    while(conn_fd[cnt] = accept(sock_fd, (struct sockaddr *)&clntaddr, (socklen_t *)&sock_len))
    {
        struct thread_args args = {0};
        args.fd = conn_fd;
        args.cnt = cnt;
        printf("Connected by Client at %s with fd %d\n", inet_ntoa(clntaddr.sin_addr), conn_fd[cnt]); 
        cnt++;
        pthread_create(&tid, NULL, ReadWrite, (void *)&args);
    }
}

