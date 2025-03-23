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
        int actual_player = room->p1->socket;
        int is_game_running = 1;
        meta_t send_packet = {.type = 0x02, .size = 0};
        int card_id = -1;
        meta_t callback;

        do
        {
            if(connfd == actual_player)
            {
                // ask actual player to play
                send(actual_player, &send_packet, sizeof(meta_t), 0);
                recv(actual_player, &callback, sizeof(meta_t), 0);

                // we receive the id of the card the player choose
                send_packet.type = 0xFF;
                recv(actual_player, &card_id, sizeof(int), 0);
                send(actual_player, &send_packet, sizeof(meta_t), 0);
            }
            // do treatment depending on the received card
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
