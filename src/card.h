#ifndef CARD_HPP
#define CARD_HPP

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
    int id, mana, power, draw;
    CARD_TYPE type;
    CARD_CLASS class;
} card_t;

#endif // CARD_HPP
