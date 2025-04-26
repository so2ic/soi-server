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
#include <sys/select.h>

#include "packet.h"
#include "game.h"
#include "player.h"
#define QUEUE_LIB_IMPLEMENTATION
#include "data_structs/queue.h"

typedef struct 
{
    room_t* room;
    queue_t* queue;
} thread_args;

void* client_handler(void* args);
void send_card(int connfd, card_t* card, room_t* room, queue_t* queue);
int check_callback(meta_t* callback);
void send_resource(int actual_player, int value, char* resource, room_t* room, queue_t* queue);
int recv_handler(int connfd, void* buf, int len, int flags);
void handle_deconnection(room_t* room, int dfd, queue_t* queue);

#endif // SERVER_H
