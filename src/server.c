#include "server.h"

/* EXPlAIN SOCKET DISCUSSION STEP BY STEP 
 * STEP ONE: wait for the room to be full
 * STEP TWO: inform both player that tere room is full (game start)
 * STEP THREE : send base deck to both player
*/
void* client_handler(void* args)
{
    thread_args* actual_args = args;
    int connfd = actual_args->connfd;
    room_t* room = actual_args->room;

    // wait for room to be full
    while(room->count < 2) {;;}

    // inform player that room is full
    {
        meta_t send_packet = {.type = 0x01, .size = 0};
        meta_t recv_packet;
        bzero(&recv_packet, sizeof(meta_t));
        send(connfd, &send_packet, sizeof(meta_t), 0); 
        recv(connfd, &recv_packet, sizeof(meta_t), 0);

        if(recv_packet.type != 0xFF)
        {
            printf("Client %d does not get message\n", connfd);
            exit(1);
            // TODO 
            // handle error properly
        }
    }

    // send base deck to clients
    {
        game_t* game = room->game;

        printf("CONNFD : %d\n", connfd);

        for(int i = 0; i < game->base_deck->count; ++i)
        {
            card_t* card = (card_t*) ll_get_data_at(game->base_deck, i);
            for(int j = 0; j < card->rarity; ++j)
            {
                send_card(connfd, card);
            }
        }
    }

    // game loop
    {
        game_t* game = room->game;
        int actual_player = room->socket_p1;
        int is_game_running = 1;

        do
        {
            if(connfd == room->socket_p1)
            {
            } 
        } 
        while(is_game_running);
    }
}

void send_card(int connfd, card_t* card)
{
    meta_t callback;
    meta_t send_packet = {.type = 0x03, .size = 0};

    // tell the client we are sending a card
    send(connfd, &send_packet, sizeof(meta_t), 0);
    recv(connfd, &callback, sizeof(meta_t), 0);
    if(!check_callback(&callback))
    {
        perror("0x03 L74");
        exit(errno);
    }

    bzero(&callback, sizeof(meta_t));
    
    // actually send the card
    send(connfd, card, sizeof(card_t), 0);
    recv(connfd, &callback, sizeof(meta_t), 0);
    if(!check_callback(&callback))
    {
        perror("SEND CARD L85");
        exit(errno);
    }

    // send the card name size
    send_packet.type = 0x04;
    send_packet.size = strlen(card->name) + 1;
    printf("CARD NAME SIZE : %zu\n", send_packet.size);
    send(connfd, &send_packet, sizeof(meta_t), 0);
    recv(connfd, &callback, sizeof(meta_t), 0);
    if(!check_callback(&callback))
    {
        perror("0x04 L97");
        exit(errno);
    }

    bzero(&callback, sizeof(meta_t));
     
    // actually send the card name
    send(connfd, card->name, strlen(card->name) + 1, 0);
    recv(connfd, &callback, sizeof(meta_t), 0);
    if(!check_callback(&callback))
    {
        perror("CARD NAME L108");
        exit(errno);
    }
}

int check_callback(meta_t* callback)
{
    return (callback->type == 0xFF) ? 1 : 0;
}
