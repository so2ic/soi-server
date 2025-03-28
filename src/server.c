#include "server.h"

#define PLAYER_COUNT 2

/* EXPlAIN SOCKET DISCUSSION STEP BY STEP 
 * STEP ONE: wait for the room to be full
 * STEP TWO: inform both player that tere room is full (game start)
 * STEP THREE : send base deck to both player
*/
void* client_handler(void* args)
{
    thread_args* actual_args = args;
    room_t* room = actual_args->room;

    player_t *players[PLAYER_COUNT] = {room->p1, room->p2};
    game_t* game = room->game;

    // giving base deck card to the player
    {
        for(int i = 0; i < PLAYER_COUNT; ++i)
        {
            players[i]->deck = (ll_t*) realloc(players[i]->deck, ll_get_size(game->base_deck) * sizeof(ll_t));
        
            int err = ll_copy(players[i]->deck, game->base_deck);
            if(err)
            {
                perror("error while copying linked list");
                exit(errno);
            }
        }
    }

    printf("debug\n");
    // draw first five random card
    {
        for(int i = 0; i < PLAYER_COUNT; ++i)
        {
            for(int j = 0; j < 5; ++j)
            {
                int r = rand() % ll_get_size(players[i]->deck);
                ll_insert(players[i]->hand, ll_get_data_at(players[i]->deck, r));
                ll_remove_at(players[i]->deck, r);
            }     
        }
    }

    // wait for room to be full
    while(room->count < 2) {;;}

    // inform player that room is full
    {
        for(int i = 0; i < PLAYER_COUNT; ++i)
        {
            int connfd = players[i]->socket;

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
    }


    // send their stats to client
    {
        // second player get 1 more mastery
        players[1]->mastery = 1;
        for(int i = 0; i < PLAYER_COUNT; ++i)
        {
            player_t player = *(players[i]);
            int connfd = player.socket;
            meta_t send_packet = (meta_t) {.type = 0x05, .size = (size_t) player.hp};
            meta_t callback;
            send(connfd, &send_packet, sizeof(meta_t), 0);
            recv(connfd, &callback, sizeof(meta_t), 0);
            if(!check_callback(&callback))
            {
                perror("sending hp");
                exit(errno);
            }

            bzero(&callback, sizeof(meta_t));
            send_packet.type = 0x06;
            send_packet.size = (size_t) player.mana;
            send(connfd, &send_packet, sizeof(meta_t), 0);
            recv(connfd, &callback, sizeof(meta_t), 0);
            if(!check_callback(&callback))
            {
                perror("sending mana");
                exit(errno); 
            }

            bzero(&callback, sizeof(meta_t));
            send_packet.type = 0x07;
            send_packet.size = player.power;
            send(connfd, &send_packet, sizeof(meta_t), 0);
            recv(connfd, &callback, sizeof(meta_t), 0);
            if(!check_callback(&callback))
            {
                perror("sending power");
                exit(errno);
            }

            bzero(&callback, sizeof(meta_t));
            send_packet.type = 0x08;
            send_packet.size = player.mastery;
            send(connfd, &send_packet, sizeof(meta_t), 0);
            recv(connfd, &callback, sizeof(meta_t), 0);
            if(!check_callback(&callback))
            {
                perror("sending mastery");
                exit(errno);
            }
        }
    }

    // send base hand to clients
    {
        for(int i = 0; i < PLAYER_COUNT; ++i)
        {
            player_t player = *(players[i]);
            int connfd = player.socket;
            for(int j = 0; j < ll_get_size(player.hand); ++j)
            {
                card_t* card = (card_t*) ll_get_data_at(player.hand, j);
                send_card(connfd, card);
            }
        }
    }

    // game loop
    {
        int actual_player = players[0]->socket;
        int is_game_running = 1;
        meta_t send_packet = {.type = 0x02, .size = 0};
        int card_id = -1;
        meta_t callback;

        do
        {
            // ask actual player to play
            send(actual_player, &send_packet, sizeof(meta_t), 0);
            recv(actual_player, &callback, sizeof(meta_t), 0);

            // we receive the id of the card the player choose
            send_packet.type = 0xFF;
            recv(actual_player, &card_id, sizeof(int), 0);
            send(actual_player, &send_packet, sizeof(meta_t), 0);
            // do treatment depending on the received card
        } 
        while(is_game_running);
    }
}

void send_card(int connfd, card_t* card)
{
    meta_t callback;
    meta_t send_packet = {.type = 0x03, .size = 0};

    bzero(&callback, sizeof(meta_t));

    // tell the client we are sending a card
    send(connfd, &send_packet, sizeof(meta_t), 0);
    recv(connfd, &callback, sizeof(meta_t), 0);
    if(!check_callback(&callback))
    {
        perror("0x03 L74");
        exit(errno);
    }

    bzero(&callback, sizeof(meta_t));
    
    /*
    // actually send the card
    send(connfd, card, sizeof(card_t), 0);
    recv(connfd, &callback, sizeof(meta_t), 0);
    if(!check_callback(&callback))
    {
        perror("SEND CARD L85");
        exit(errno);
    }
    */

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
