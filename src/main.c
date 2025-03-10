#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

#include "packet.h"
#include "server.h"

#define PORT 5087
#define SA struct sockaddr

void* chat(void* args)
{
    meta_t meta_data;
    char buffer[80];
    thread_args* actual_args = args;
    int connfd = actual_args->connfd;

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
    pthread_t* threads_id = (pthread_t*) malloc(sizeof(pthread_t));

    int sockfd, connfd, len;
    int client_number = 0;
    struct sockaddr_in sockserve, cli;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket()");
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

    while(1)
    {
        connfd = accept(sockfd, (struct sockaddr*)&cli, (socklen_t *)&len);    
        if(connfd < 0)
        {
            printf("accept failed\n");
            return 1;
        }
        printf("New client connected : %d\n", connfd);

        ++client_number;
        threads_id = (pthread_t*) realloc(threads_id, client_number * sizeof(pthread_t));
        thread_args args = {.connfd = connfd};

        pthread_create(&threads_id[client_number - 1], NULL, chat, &args);  
    }

    close(sockfd);
    return 0;
}
