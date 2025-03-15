#ifndef GAME_HPP
#define GAME_HPP

#include "room.h"

typedef struct
{
   room_t room; 
   card_t **deck;
} game_t;

#endif // GAME_HPP
