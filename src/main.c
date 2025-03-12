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
#include "json/parser.h"

#define PORT 5087
#define SA struct sockaddr

void* chat(void* args)
{
    meta_t meta_data;
    char buffer[80];
    thread_args* actual_args = args;
    int connfd = actual_args->connfd;
    room_t* room = actual_args->room;

    printf("waiting for opponent\n");
    while(room->count < 2);

    int ennemy = room->player1 == connfd ? room->player2 : room->player1;
    printf("ennemy found : %d\n", ennemy);
    for(;;)
    {
        recv(connfd, &buffer, sizeof(buffer), 0);

        printf("message from client : %s\n", buffer);

        send(ennemy, &buffer, strlen(buffer) + 1, 0);
        char* msg = "Message get";
        send(connfd, msg, strlen(msg) + 1, 0);
        printf("Message sent");
    }
}

int main(int argc, char** argv)
{
    char* file_path = "/Users/vincentastolfi/git/soi/soi-server/src/card/cards.json";
    json_to_cards(file_path);
    // Code to start the server
    // listen to socket and start threads
    // uncomment after json parser is working 
    /*
    pthread_t* threads_id = (pthread_t*) malloc(sizeof(pthread_t));
    room_t* room = (room_t*) malloc(sizeof(room_t));
    room->count = 0;

    int sockfd, connfd, len;
    int client_count = 0;
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

        // add new player to the room
        if (room->count == 0)
            room->player1 = connfd;
        else
            room->player2 = connfd;
        room->count++;

        // create thread for new client
        ++client_count;
        threads_id = (pthread_t*) realloc(threads_id, client_count * sizeof(pthread_t));
        thread_args args = {.connfd = connfd, .room = room};

        pthread_create(&threads_id[client_count - 1], NULL, chat, &args);  
    }

    close(sockfd);
    */
    return 0;
}
