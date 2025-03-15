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
        meta_t packet = {.type = 0x01, .size = 0};
        send(connfd, &packet, sizeof(meta_t), 0); 
    }

    for(;;) {}
}
