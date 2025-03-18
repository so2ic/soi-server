#include "server.h"

/* EXPlAIN SOCKET DISCUSSION STEP BY STEP 
 * STEP ONE: wait for the room to be full
 * STEP TWO: inform both player that tere room is full (game start)
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

        // tell the client how many card we are going to send
        meta_t size_info = {.type = 0x02, .size = (size_t) game->base_deck->count};
        meta_t callback;
        send(connfd, &size_info, sizeof(meta_t), 0);
        recv(connfd, &callback, sizeof(meta_t), 0);

        if(callback.type != 0xFF)
        {
            printf("client doesn't receive deck size\n");
            exit(1);
        }

        for(int i = 0; i < game->base_deck->count; ++i)
        {
            card_t* card = (card_t*) ll_get_data_at(game->base_deck, i);
            printf("sending card : %s\n", card->name);
            for(int j = 0; j < card->rarity; ++j)
            {
                meta_t callback;
                send(connfd, card, sizeof(card_t), 0);
                recv(connfd, &callback, sizeof(meta_t), 0);
                
                if(callback.type != 0xFF)
                {
                    printf("card not receive by client\n");
                    exit(1); 
                }
            }
        }
    }

    for(;;) 
    {
        sleep(1);
    }
}
