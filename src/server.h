#ifndef SERVER_H
#define SERVER_H

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
#include "player.h"

typedef struct 
{
    room_t* room;
} thread_args;

void* client_handler(void* args);
void send_card(int connfd, card_t* card);
int check_callback(meta_t* callback);
void send_resource(int actual_player, int value, char* resource);

#endif // SERVER_H
