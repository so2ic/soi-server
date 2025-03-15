#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "packet.h"

typedef struct 
{
    int player1, player2, count;
} room_t;

typedef struct 
{
    int connfd;
    room_t* room;
} thread_args;

void* client_handler(void* args);

#endif // SERVER_HPP
