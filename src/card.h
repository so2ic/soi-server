#ifndef CARD_H
#define CARD_H

#include <stdlib.h>
#include <string.h>

#include "player.h"

typedef enum CARD_TYPE
{
    ALLIES,
    MERCENARIES,
    CHAMPION,
} CARD_TYPE;

typedef enum CARD_CLASS
{
    BASE_DECK,
    ORDER,
    WRAETHE,
    UNDERGROWTH,
    HOMODEUS,
} CARD_CLASS;

typedef enum
{
    BASE_TYPE,
    INSTEAD,
} EFFECT_TYPE;

typedef enum
{
    BASE_NEED,
    MASTERY,
} EFFECT_NEED;

typedef enum
{
    MANA,
    DRAW,
    POWER,
} EFFECT_GAIN;

typedef struct
{
    EFFECT_GAIN resource;
    int value;
} effect_gain_t;

typedef struct
{
    EFFECT_NEED resource;
    int value;
} effect_condition_t;

typedef struct 
{
    EFFECT_TYPE type;
    effect_gain_t gain;
    effect_condition_t condition; 
} effect_t;

typedef struct card_t 
{
    char* name;
    int id, rarity, effect_count;
    effect_t **effects;
    CARD_TYPE type;
    CARD_CLASS class;
} card_t;

void card_copy(card_t* new, card_t* c);
EFFECT_GAIN process_card(card_t* card, player_t* player);

#endif // CARD_HPP
