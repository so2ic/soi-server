#ifndef GAME_H
#define GAME_H

#include "card.h"
#include "data_structs/linked_list.h"
#include "player.h"

typedef struct
{
   ll_t* draw;
   ll_t* base_deck;
} game_t;

typedef struct 
{
    player_t *p1, *p2;
    int count;
    game_t* game;
} room_t;

#endif // GAME_H
