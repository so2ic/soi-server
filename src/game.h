#ifndef GAME_HPP
#define GAME_HPP

#include "card.h"

typedef struct
{
    int socket;
    card_t **deck;
    card_t **discard; 
} player_t;

typedef struct
{
   card_t **draw;
   card_t **base_deck;
} game_t;

typedef struct 
{
    player_t player1, player2;
    int count;
    game_t game;
} room_t;

#endif // GAME_HPP
