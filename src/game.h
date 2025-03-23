#ifndef GAME_HPP
#define GAME_HPP

#include "card.h"
#include "data_structs/linked_list.h"

typedef struct
{
   ll_t* draw;
   ll_t* base_deck;
} game_t;

typedef struct
{
    int socket, mana, hp, mastery, power;
} player_t;

typedef struct 
{
    player_t *p1, *p2;
    int count;
    game_t* game;
} room_t;

#endif // GAME_HPP
