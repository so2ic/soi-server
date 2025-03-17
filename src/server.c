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

    for(;;) {}
}
