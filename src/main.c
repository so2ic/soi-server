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
#include <time.h>

#include "packet.h"
#include "server.h"
#include "json/parser.h"
#include "data_structs/linked_list.h"
#include "utils/dotenv.h"

#define QUEUE_LIB_IMPLEMENTATION
#include "data_structs/queue.h"

#define PORT 5093
#define SA struct sockaddr
#define CARD_NUMBER 4

int main(int argc, char** argv)
{
    card_t** cards = NULL;
    pthread_t* threads_id = (pthread_t*) malloc(sizeof(pthread_t));
    room_t** rooms = (room_t**) malloc(sizeof(room_t));
    int sockfd, connfd, len;
    int room_count = 0;
    struct sockaddr_in sockserve, cli;
    game_t* game = (game_t*) malloc(sizeof(game_t));
    queue_t* queue = queue_init();

    srand(time(NULL));

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
        char* file_path = "./src/card/cards.json";
        // only works if we already know how much cards are describe in json file
        cards = (card_t**) calloc(CARD_NUMBER, sizeof(card_t));
        int err = json_to_cards(file_path, cards);
        if(err)
            printf("error parsing");


        // printf("\n\n----PARSING FINISHED----\n\n");
    }

    // we store cards in a game info object
    {
        ll_t* base_deck = ll_init();
        ll_t* draw = ll_init();

        for(int i = 0; i < CARD_NUMBER; ++i)
        {
            for(int j = 0; j < cards[i]->rarity; ++j)
            {
                if(cards[i]->type == (CARD_TYPE) BASE_DECK) 
                    ll_insert(base_deck, cards[i]);
                else
                    ll_insert(draw, cards[i]);
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
        queue_add(queue, p);

        p->socket = connfd;
        p->mana = 0;
        p->hp = 50;
        p->mastery = 0;
        p->power = 0;
#ifdef DEBUG
        p->power = 50;
#endif
        p->hand = ll_init();
        p->deck = ll_init();
        p->discard = ll_init();

        // two player are waiting => we place them in a room
        if(queue->size == 2)
        {
            rooms[room_count]->p1 = (player_t*) queue_dequeue(queue);
            rooms[room_count]->p2 = (player_t*) queue_dequeue(queue);

            ++room_count;
            rooms = (room_t**) realloc(rooms, (room_count+1) * sizeof(room_t)); 
            rooms[room_count] = (room_t*) malloc(sizeof(room_t));
            rooms[room_count]->count = 0;
            rooms[room_count]->game = game;

            thread_args* args = malloc(sizeof(thread_args));
            args->room = rooms[room_count - 1];

            threads_id = (pthread_t*) realloc(threads_id, room_count * sizeof(pthread_t));

            pthread_create(&threads_id[room_count - 1], NULL, client_handler, args);  
        }
    }

    close(sockfd);
    for(int i = 0; i < 2; ++i)
        free(cards[i]);
    free(cards);
    return 0;
}
