#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>

#include "packet.h"

#define PORT 5087
#define SA struct sockaddr

void chat(int connfd)
{
    meta_t meta_data;
    char buffer[80];

    for(;;)
    {
        recv(connfd, &buffer, sizeof(buffer), 0);

        printf("message from client : %s\n", buffer);

        char* msg = "Message get";
        send(connfd, msg, strlen(msg) + 1, 0);
        printf("Message sent");
    }
}

int main(int argc, char** argv)
{
    int sockfd, connfd, len;
    struct sockaddr_in sockserve, cli;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket()");
        printf("Socket creation failed\n");
        return 1;
    }
    else
        printf("Socker created\n");
    bzero(&sockserve, sizeof(sockserve));

    sockserve.sin_family = AF_INET;
    sockserve.sin_port = htons(PORT);
    inet_aton("127.0.0.1", (struct in_addr*)&sockserve.sin_addr.s_addr);

    if(bind(sockfd, (struct sockaddr*)&sockserve, sizeof(sockserve)) != 0)
    {
        perror("bind()");
        return -1; 
    }

    if(listen(sockfd, 5) != 0)
    {
        printf("listen failed\n"); 
    }

    len = sizeof(cli);

    connfd = accept(sockfd, (struct sockaddr*)&cli, (socklen_t *)&len);    
    if(connfd < 0)
    {
        printf("accept failed\n");
        return 1;
    }
    printf("New client connected : %d", connfd);

    chat(connfd);

    close(sockfd);
    return 0;
}
