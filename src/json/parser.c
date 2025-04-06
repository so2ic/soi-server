#include "parser.h"

// Define the global variables
int m_index = 0;
char* m_buffer = NULL;
char* m_content = NULL;

// Parsing function
int json_to_cards(const char* file_path, card_t** cards)
{
    FILE* file;
    long length;
    optionnal_char_t c;
    size_t deck_size = 0;
    char *last_token = NULL, *dump;
    int is_effect = 0, effect_count = 0, is_condition = 0;

    file = fopen(file_path, "rb");
    if (file == NULL)
    {
        perror("Error reading file");
        return 1; // Handle error
    }

    // Copy file content into a string
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    m_content = malloc(length + 1);
    if (m_content)
    {
        size_t bytes_read = fread(m_content, 1, length, file);
        m_content[bytes_read] = '\0'; // Null-terminate the string
    }
    else
    {
        fclose(file);
        return 1; // Handle memory allocation failure
    }

    fclose(file);

    while((c = _peek()).has_value)
    {
        if(isdigit(c.value))
        {
            int number_length = 0;
            while((c = _peek(number_length)).has_value)
            {
                if(!isdigit(c.value))
                    break;

                ++number_length;
            }

            m_buffer = _consume(number_length);

            // we are strating the effects reading
            if(strcmp(last_token,"count") == 0)
            {
                if(is_condition)
                {
                    cards[deck_size]->effects[effect_count]->condition.value = atoi(m_buffer);
                }
                else
                {
                    cards[deck_size]->effects = (effect_t**) calloc(atoi(m_buffer), sizeof(effect_t)); 
                    cards[deck_size]->effect_count = atoi(m_buffer); 
                }
            }

            if(strcmp(last_token,"id") == 0)
                    cards[deck_size]->id = atoi(m_buffer);
            if(strcmp(last_token,"rarity") == 0)
                    cards[deck_size]->rarity = atoi(m_buffer);

            if(strcmp(last_token,"mana") == 0)
            {
                cards[deck_size]->effects[effect_count]->gain.resource = (EFFECT_GAIN) MANA;
                cards[deck_size]->effects[effect_count]->gain.value = atoi(m_buffer);
            }
            if(strcmp(last_token,"power") == 0)
            {
                cards[deck_size]->effects[effect_count]->gain.resource = (EFFECT_GAIN) POWER;
                cards[deck_size]->effects[effect_count]->gain.value = atoi(m_buffer);
            }
            if(strcmp(last_token,"draw") == 0)
            {
                cards[deck_size]->effects[effect_count]->gain.resource = (EFFECT_GAIN) DRAW;
                cards[deck_size]->effects[effect_count]->gain.value = atoi(m_buffer);
            }

            free(m_buffer);
            free(last_token);
            m_buffer = NULL;
            last_token = NULL;
        }
        switch(c.value)
        {
            case ' ':
                dump = _consume();
                free(dump);
                break;
            case '\n':
                dump = _consume();
                free(dump);
                break;
            case '{':
               if (m_index == 0)
               {
                    dump = _consume();
                    free(dump);
               } 
               else 
               {
                   if(is_effect)
                   {
                        dump = _consume();
                        free(dump);
                   }
                   else 
                   {
                        cards[deck_size] = (card_t*) malloc(sizeof(card_t));
                        cards[deck_size]->name = malloc(strlen(last_token) + 1);
                        strcpy(cards[deck_size]->name, last_token);
                        free(last_token);
                        last_token = NULL;
                        dump = _consume();
                        free(dump);
                        //printf("NAME : %s\n", cards[deck_size]->name);
                   }
               }
               break;
            case '}':
               if(is_effect)
               {
                    dump = _consume();
                    free(dump);
               }
               else
               {
                    // end of card 
                    optionnal_char_t next_field;
                    int field_length = 1;
                    while((next_field = peek(field_length)).has_value)
                    {
                        // there is another card
                        if(next_field.value == '"') 
                        {
                            ++deck_size;
                            // had to be improved
                            //cards = realloc(cards, (deck_size+1) * sizeof(card_t)); 
                            break;
                        }

                        // this is the last card
                        if(next_field.value == '}')
                            break;
                        ++field_length;
                    }
                    dump = _consume();
                    free(dump);
               }
               break;
            case '"':
               // is m_buffer isn't null, then this is a closing 
               if(m_buffer != NULL) 
               {
                    if(_peek(1).value == ':')
                    {
                        if(last_token != NULL)
                        {
                            free(last_token);
                            last_token = NULL;
                        }

                        last_token = malloc(strlen(m_buffer + 1));
                        strcpy(last_token, m_buffer);
                        free(m_buffer);
                        m_buffer = NULL;
                        dump = _consume(2);
                        free(dump);
                        //printf("TOKEN : %s\n", last_token);
                    }
                    else
                    {

                        if(strcmp(last_token, "class") == 0)
                        {
                            if(strcmp(m_buffer, "allies") == 0)
                              cards[deck_size]->type = (CARD_TYPE) ALLIES;  

                            if(strcmp(m_buffer, "mercenaries") == 0)
                              cards[deck_size]->type = (CARD_TYPE) ALLIES;  

                            if(strcmp(m_buffer, "champion") == 0)
                              cards[deck_size]->type = (CARD_TYPE) ALLIES;  
                        }

                        if(strcmp(last_token, "type") == 0)
                        {
                            if(is_effect)
                            {
                                if(strcmp(m_buffer, "instead") == 0)
                                    cards[deck_size]->effects[effect_count]->type =
                                        (EFFECT_TYPE) INSTEAD;
                            } 
                            else
                                if(strcmp(m_buffer, "base") == 0)
                                    cards[deck_size]->class = (CARD_CLASS) BASE_DECK;
                        }

                        if(strcmp(last_token, "need") == 0)
                        {
                            if(strcmp(m_buffer, "none") == 0)
                            {
                                cards[deck_size]->effects[effect_count]->type = 
                                    (EFFECT_TYPE) BASE_TYPE;
                                cards[deck_size]->effects[effect_count]->condition.resource = 
                                    (EFFECT_NEED) BASE_NEED;
                                cards[deck_size]->effects[effect_count]->condition.value = -1;
                            }
                            if(strcmp(m_buffer, "mastery") == 0)
                            {
                                cards[deck_size]->effects[effect_count]->condition.resource =
                                    (EFFECT_NEED) MASTERY;
                            }
                        }

                        dump = _consume();
                        free(dump);
                        free(last_token);
                        free(m_buffer);
                        m_buffer = NULL;
                        last_token = NULL;
                    }
               }
               else 
               {
                    dump = _consume();
                    free(dump);
                    int field_length = 0;
                    optionnal_char_t field;
                    while((field = _peek(field_length)).has_value)
                    {
                        // End of the field
                        if(field.value == '"')
                        {
                            break;
                        }
                        ++field_length;
                    }

                    m_buffer = _consume(field_length);
                    //printf("BUFFER : %s\n", m_buffer);
               }
               break;
            case '[':
               if(is_effect)
               {
                   cards[deck_size]->effects[effect_count] = (effect_t*) malloc(sizeof(effect_t));
                   is_condition = 1;
               }
               else
                   is_effect = 1;

               dump = _consume();
               free(dump);
               break;
            case ']':
               if(is_effect && cards[deck_size]->effect_count == effect_count)
               {
                   is_effect = 0;
                   effect_count = 0; 
               }
               else
               {
                   is_condition = 0;
                   effect_count++;
               }
               dump = _consume();
               free(dump);
               break;
            case ',':
               dump = _consume();
               free(dump);
               break;

            default:
               printf("ERROR SHOULDN'T HAPPEND\n");
               printf("ERROR ON : %d, %c", m_index, c.value);
               break;
        }
    }

    free(m_buffer);
    free(m_content);
    m_content = NULL;
    m_buffer = NULL;

    return 0;
}

optionnal_char_t peek(int i)
{
    if(m_index + i >= strlen(m_content))
        return (optionnal_char_t) {.has_value = 0};
    else
        return (optionnal_char_t) {.value = m_content[m_index + i], .has_value = 1};
}

char* consume(int i)
{
    if(i == 0)
        return "ERROR"; // to handle properly
    char* s = malloc(i + 1);
    for(int j = 0; j < i; ++j)
    {
        s[j] = m_content[m_index];
        ++m_index;
    }

    s[i] = '\0';
    return s;
}
