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

EFFECT_GAIN process_card(card_t* card, player_t* player)
{
    EFFECT_GAIN ret;
    for(int i = 0; i < card->effect_count; ++i)
    {
        EFFECT_NEED needed_resource = card->effects[i]->condition.resource;
        int condition_value = card->effects[i]->condition.value;
        
        if(needed_resource == (EFFECT_NEED) MASTERY)
            if(player->mastery < condition_value)
                continue;

        if(card->effects[i]->type == (EFFECT_TYPE) BASE_TYPE || card->effects[i]->type == INSTEAD)
        {
            int gained_value = card->effects[i]->gain.value;  
            EFFECT_GAIN gained_resource = card->effects[i]->gain.resource;

            if(gained_resource == (EFFECT_GAIN) MANA)
            {
                player->mana += gained_value;
                ret = (EFFECT_GAIN) MANA;
            }

            if(gained_resource == (EFFECT_GAIN) POWER)
            {
                player->power += gained_value;
                ret = (EFFECT_GAIN) POWER; 
            }

            // TODO
            // Implement drawing card  
        }
    }
    
    return ret;
}
