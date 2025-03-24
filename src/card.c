#include "card.h"

#include <stdio.h>

void card_copy(card_t* new, card_t* c)
{
    new->name = malloc(strlen(c->name) + 1);
    strcpy(new->name, c->name);
    /*
    new->id = c->id; 
    new->mana = c->mana;
    new->power = c->power;
    new->draw = c->draw;
    new->rarity = c->rarity;
    new->type = c->type;
    new->class = c->class;
    */
}
