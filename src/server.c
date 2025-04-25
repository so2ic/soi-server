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

    // inform player that room is full
    {
        for(int i = 0; i < PLAYER_COUNT; ++i)
        {
            int connfd = players[i]->socket;

            meta_t send_packet = {.type = 0x01, .size = 0};
            meta_t recv_packet;
            bzero(&recv_packet, sizeof(meta_t));
            send(connfd, &send_packet, sizeof(meta_t), 0); 
            if(!recv_handler(connfd, &recv_packet, sizeof(meta_t), 0))
            {
                handle_deconnection(room, connfd);
                pthread_exit(NULL);
            }

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
            send_resource(connfd, player.hp, "hp", room);
            send_resource(connfd, player.mastery, "mastery", room);
            send_resource(connfd, player.mana, "mana", room);
            send_resource(connfd, player.power, "power", room);
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
                send_card(connfd, card, room);
            }
        }
    }

    // game loop
    {
        int actual_player = players[0]->socket;
        int actual_player_place = 0;
        int is_game_running = 1;
        meta_t send_packet;
        meta_t callback;

        do
        {
            send_packet.type = 0x02;
            send_packet.size = 0;
            
            // ask actual player to play
            send(actual_player, &send_packet, sizeof(meta_t), 0);
            if(!recv_handler(actual_player, &callback, sizeof(meta_t), 0))
            {
                handle_deconnection(room, actual_player);
                pthread_exit(NULL);
            }

            bzero(&callback, sizeof(meta_t));

            if(!recv_handler(actual_player, &callback, sizeof(meta_t), 0))
            {
                handle_deconnection(room, actual_player);
                pthread_exit(NULL);
            }

            // we received the played card
            if(callback.type == 0x03)
            {
                int card_id;
                player_t *player = players[actual_player_place];

                send_packet.type = 0xff;
                send(actual_player, &send_packet, sizeof(meta_t), 0);
                 
                bzero(&callback, sizeof(meta_t));
                if(!recv_handler(actual_player, &card_id, sizeof(int), 0))
                {
                    handle_deconnection(room, actual_player);
                    pthread_exit(NULL);
                }
                send(actual_player, &send_packet, sizeof(meta_t), 0);
                bzero(&send_packet, sizeof(meta_t));
                
                for(int i = 0; i < ll_get_size(player->hand); ++i)
                {
                    // We found the first occurence of our card id
                    if(((card_t*)ll_get_data_at(player->hand, i))->id == card_id)
                    {
                        // TODO
                        // handle different resource changes
                        EFFECT_GAIN gain = process_card(ll_get_data_at(player->hand, i), player); 
                        ll_insert(player->discard, ll_get_data_at(player->hand, i));
                        ll_remove_at(player->hand, i);

                        send_packet.type = 0x0A;
                        send_packet.size = (size_t) i;
                        
                        send(actual_player, &send_packet, sizeof(meta_t), 0);
                        if(!recv_handler(actual_player, &callback, sizeof(meta_t), 0))
                        {
                            handle_deconnection(room, actual_player);
                             pthread_exit(NULL);
                        }
                        if(!check_callback(&callback))
                        {
                            perror("0x0A remove card"); 
                            exit(errno);
                        }

                        bzero(&callback, sizeof(meta_t));
                        if(gain == (EFFECT_GAIN) MANA)
                            send_resource(actual_player, players[actual_player_place]->mana, "mana", room);
                        else if(gain == (EFFECT_GAIN) POWER)
                            send_resource(actual_player, players[actual_player_place]->power, "power", room);
                        break;
                    } 
                }
            }
            else if(callback.type == 0x0B)
            {
                player_t* player = players[actual_player_place];
                send_packet.type = 0xFF;

                send(actual_player, &send_packet, sizeof(meta_t), 0);

                // discard every card left in hand
                if(ll_get_size(player->hand) > 0)
                {
                    meta_t callback;
                    ll_copy(player->discard, player->hand);
                    ll_empty(player->hand); 
                    send_packet.type = 0x0E;
                    send(actual_player, &send_packet, sizeof(meta_t), 0);
                    if(!recv_handler(actual_player, &callback, sizeof(meta_t), 0))
                    {
                        handle_deconnection(room, actual_player);
                         pthread_exit(NULL);
                    }
                    if(!check_callback(&callback))
                    {
                        perror("Error on empty card 0x0E");
                        exit(errno);
                    }
                }

                // redraw hand
                while(ll_get_size(player->hand) != 5)
                {
                    if(ll_get_size(player->deck) > 0)
                    {
                        int r = rand() % ll_get_size(player->deck);
                        ll_insert(player->hand, ll_get_data_at(player->deck, r));
                        ll_remove_at(player->deck, r);
                    }
                    else
                    {
                        ll_copy(player->deck, player->discard);
                        ll_empty(player->discard);
                    }
                }

                for(int j = 0; j < ll_get_size(player->hand); ++j)
                {
                    card_t* card = (card_t*) ll_get_data_at(player->hand, j);
                    send_card(actual_player, card, room);
                }

                actual_player_place = actual_player_place == 0 ? 1 : 0;
                actual_player = players[actual_player_place]->socket;

                bzero(&callback, sizeof(meta_t));
            }
            else if(callback.type == 0x0C)
            {
                send_packet.type = 0xFF;

                send(actual_player, &send_packet, sizeof(meta_t), 0);

                // Do I need to handle differently if mana == 0 ?

                if(players[actual_player_place]->mana > 0)
                {
                    players[actual_player_place]->mastery += 1;
                    players[actual_player_place]->mana -= 1; 
                    send_resource(actual_player, players[actual_player_place]->mana, "mana", room);
                    send_resource(actual_player, players[actual_player_place]->mastery, "mastery", room);
                }
                bzero(&callback, sizeof(meta_t));
            }
            else if(callback.type == 0x0D)
            {
                send_packet.type = 0xFF;

                send(actual_player, &send_packet, sizeof(meta_t), 0);

                player_t* en = players[actual_player_place == 0 ? 1 : 0];
                en->hp -= (int) callback.size;
                send_resource(en->socket, en->hp, "hp", room);

                bzero(&callback, sizeof(meta_t));
            }
        } 
        while(is_game_running);
    }
}

void send_card(int connfd, card_t* card, room_t* room)
{
    meta_t callback;
    meta_t send_packet = {.type = 0x03, .size = 0};

    bzero(&callback, sizeof(meta_t));

    // tell the client we are sending a card
    send(connfd, &send_packet, sizeof(meta_t), 0);
    if(!recv_handler(connfd, &callback, sizeof(meta_t), 0))
    {
        handle_deconnection(room, connfd);
        pthread_exit(NULL);
    }
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
    
    // send the card id
    send_packet.type = 0x09;
    send_packet.size = (size_t) card->id;
    send(connfd, &send_packet, sizeof(meta_t), 0);
    if(!recv_handler(connfd, &callback, sizeof(meta_t), 0))
    {
        handle_deconnection(room, connfd);
        pthread_exit(NULL);
    }
    if(!check_callback(&callback))
    {
        perror("0x09 send id"); 
        exit(errno);
    }

    // send the card name size
    send_packet.type = 0x04;
    send_packet.size = strlen(card->name) + 1;
    send(connfd, &send_packet, sizeof(meta_t), 0);
    if(!recv_handler(connfd, &callback, sizeof(meta_t), 0))
    {
        handle_deconnection(room, connfd);
        pthread_exit(NULL);
    }
    if(!check_callback(&callback))
    {
        perror("0x04 L97");
        exit(errno);
    }

    bzero(&callback, sizeof(meta_t));
     
    // actually send the card name
    send(connfd, card->name, strlen(card->name) + 1, 0);
    if(!recv_handler(connfd, &callback, sizeof(meta_t), 0))
    {
        handle_deconnection(room, connfd);
        pthread_exit(NULL);
    }
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

void send_resource(int actual_player, int value, char* resource, room_t* room)
{
    meta_t send_packet;
    meta_t callback;
    send_packet.size = (size_t) value;
    bzero(&callback, sizeof(meta_t));

    if(strcmp(resource, "mana") == 0)
        send_packet.type = 0x06;
    else if(strcmp(resource, "mastery") == 0)
        send_packet.type = 0x08;
    else if(strcmp(resource, "hp") == 0)
        send_packet.type = 0x05;
    else if(strcmp(resource, "power") == 0)
        send_packet.type = 0x07;
    else
    {
        perror("unknown resource");
        exit(errno);
    }

    send(actual_player, &send_packet, sizeof(meta_t), 0);
    if(!recv_handler(actual_player, &callback, sizeof(meta_t), 0))
    {
        handle_deconnection(room, actual_player);
        pthread_exit(NULL);
    }
    if(!check_callback(&callback))
    {
        perror("sending resource");
        exit(errno); 
    }

}

int recv_handler(int connfd, void* buf, int len, int flags)
{
    int bytes = recv(connfd, buf, len, flags);
    if(bytes == 0)
    {
        close(connfd);
       // TODO
       // free all memory related to the disconnected client 
       return 0;
    }
    else if(bytes < 0)
    {
        perror("error on recv");
        exit(errno);
    }
    
    return 1;
}

void handle_deconnection(room_t* room, int dfd)
{
    // dfd -> client that disconnect

    // ask opponent to reset
    {
        meta_t p = {.type = 0x0F, .size = 0};
        send(room->p1->socket == dfd ? room->p2->socket : room->p1->socket, &p, sizeof(meta_t), 0);
    }
}
