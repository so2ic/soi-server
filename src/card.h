#ifndef CARD_HPP
#define CARD_HPP

#include <stdlib.h>
#include <string.h>

typedef enum CARD_TYPE
{
    ALLIES,
    MERCENARIES,
    CHAMPION,
} CARD_TYPE;

typedef enum CARD_CLASS
{
    BASE_DECK, // for base decks
    ORDER,
    WRAETHE,
    UNDERGROWTH,
    HOMODEUS,
} CARD_CLASS;

typedef struct card_t 
{
    char* name;
    int id, mana, power, draw, rarity;
    CARD_TYPE type;
    CARD_CLASS class;
} card_t;

void card_copy(card_t* new, card_t* c);

#endif // CARD_HPP
