#ifndef PLAYER_H
#define PLAYER_H

#include "data_structs/linked_list.h"

typedef struct
{
    int socket, mana, hp, mastery, power;

    ll_t *hand, *deck;
} player_t;

#endif // PLAYER_H
