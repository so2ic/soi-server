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
#include "data_structs/linked_list.h"

#define PORT 5087
#define SA struct sockaddr
#define CARD_NUMBER 2

int main(int argc, char** argv)
{
    card_t** cards = NULL;
    pthread_t* threads_id = (pthread_t*) malloc(sizeof(pthread_t));
    room_t* room = (room_t*) malloc(sizeof(room_t));
    int sockfd, connfd, len;
    int client_count = 0, deck_size = 0, draw_size = 0;
    struct sockaddr_in sockserve, cli;

    // loads cards from the json file
    {
        char* file_path = "/Users/vincentastolfi/git/soi/soi-server/src/card/cards.json";
        // only works if we already know how much cards are describe in json file
        cards = (card_t**) calloc(CARD_NUMBER, sizeof(card_t));
        int err = json_to_cards(file_path, cards);
        if(err)
            printf("error parsing");


        printf("\n\n----PARSING FINISHED----\n\n");

        for(int i = 0; i < CARD_NUMBER; ++i)
        {
            if(cards[i]->type == (CARD_TYPE) BASE_DECK)
                ++deck_size;
            else
                ++draw_size;
        }

    }

    room->count = 0;
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
        player_t* p = (player_t*) malloc(sizeof(player_t));
        connfd = accept(sockfd, (struct sockaddr*)&cli, (socklen_t *)&len);    
        if(connfd < 0)
        {
            printf("accept failed\n");
            return 1;
        }
        printf("New client connected : %d\n", connfd);
        p->socket = connfd;
        p->mana = 0;
        p->hp = 50;
        p->mastery = 0;
        p->power = 0;

        // add new player to the room
        if (room->count == 0)
            room->p1 = p;
        else
        {
            ll_t* base_deck = ll_init();
            ll_t* draw = ll_init();

            int card_in_deck = 0, card_in_draw = 0;
            for(int i = 0; i < CARD_NUMBER; ++i)
            {
                if(cards[i]->type == (CARD_TYPE) BASE_DECK) 
                {
                    card_t* temp = (card_t*) malloc(sizeof(card_t));
                    card_copy(temp, cards[i]);
                    ll_insert(base_deck, temp);
                }
                else
                {
                    card_t* temp = (card_t*) malloc(sizeof(card_t));
                    card_copy(temp, cards[i]);
                    ll_insert(draw, temp);
                }

            }
 
            room->p2 = p;
            game_t g = (game_t) {.draw = draw, .base_deck = base_deck};
            room->game = &g;
        }

        room->count++;

        // create thread for new client
        ++client_count;
        threads_id = (pthread_t*) realloc(threads_id, client_count * sizeof(pthread_t));

        thread_args* args = malloc(sizeof(thread_args));
        args->connfd = connfd;
        args->room = room;

        pthread_create(&threads_id[client_count - 1], NULL, client_handler, args);  
    }

    close(sockfd);
    for(int i = 0; i < 2; ++i)
        free(cards[i]);
    free(cards);
    return 0;
}
