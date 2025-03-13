#ifndef PARSER_HPP
#define PARSER_HPP

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "../card.h"

typedef struct
{
    char value;
    int has_value;
} optionnal_char_t;

card_t* json_to_cards(const char* file_path, card_t* card);
int m_index;
char* m_buffer;
char* m_content;

optionnal_char_t peek();
char consume();

#endif // PARSER_HPP
