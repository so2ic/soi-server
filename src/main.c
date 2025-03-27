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
#include "utils/dotenv.h"

#define PORT 5087
#define SA struct sockaddr
#define CARD_NUMBER 2

int main(int argc, char** argv)
{
    card_t** cards = NULL;
    pthread_t* threads_id = (pthread_t*) malloc(sizeof(pthread_t));
    room_t** rooms = (room_t**) malloc(sizeof(room_t));
    int sockfd, connfd, len;
    int client_count = 0, deck_size = 0, draw_size = 0, room_count = 0;
    struct sockaddr_in sockserve, cli;
    game_t* game = (game_t*) malloc(sizeof(game_t));

    // loading .env variable
    {
        char* path = ".";
        int err = load_env(path);
        if(err)
        {
            perror("can't find .env file");
            exit(errno);         
        }

        char* env = get_env("ENV");
        char* version = get_env("VERSION");
        if(strcmp(env, "DEBUG") == 0)
        {
#define DEBUG
            printf("SOI2C SERVER DEBUG VERSION %s\n\n", version);
        }
    }

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

    // we store cards in a game info object
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

        game->base_deck = base_deck;
        game->draw = draw;
    }

    rooms[room_count] = (room_t*) malloc(sizeof(room_t));
    rooms[room_count]->count = 0;
    rooms[room_count]->game = game;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket()");
        return 1;
    }
    else
        printf("Socket created\n");
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
        p->hand = (int*) calloc(5, sizeof(int));
        p->deck = (int*) calloc(10, sizeof(int));

        // add new player to the room
        if (rooms[room_count]->count == 0)
            rooms[room_count]->p1 = p;
        else
        {
            rooms[room_count]->p2 = p;
        }

        thread_args* args = malloc(sizeof(thread_args));
        args->connfd = connfd;
        args->room = rooms[room_count];

        if(++(rooms[room_count]->count) == 2)
        {
            ++room_count;
            rooms = (room_t**) realloc(rooms, (room_count+1) * sizeof(room_t)); 
            rooms[room_count] = (room_t*) malloc(sizeof(room_t));
            rooms[room_count]->count = 0;
            rooms[room_count]->game = game;
        }

        // create thread for new client
        ++client_count;
        threads_id = (pthread_t*) realloc(threads_id, client_count * sizeof(pthread_t));

        pthread_create(&threads_id[client_count - 1], NULL, client_handler, args);  
    }

    close(sockfd);
    for(int i = 0; i < 2; ++i)
        free(cards[i]);
    free(cards);
    return 0;
}
