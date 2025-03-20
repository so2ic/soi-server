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
#include <stdlib.h>

#include "packet.h"
#include "game.h"

typedef struct 
{
    int connfd;
    room_t* room;
} thread_args;

void* client_handler(void* args);
void send_card(int connfd, card_t* card);
int check_callback(meta_t* callback);

#endif // SERVER_HPP
