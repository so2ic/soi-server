#ifndef PARSER_HPP
#define PARSER_HPP

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "../card.h"

typedef struct {
    char value;
    int has_value;
} optionnal_char_t;

card_t* json_to_cards(const char* file_path, card_t* card);
optionnal_char_t peek(int i);
char* consume(int i);

// Global variables
extern int m_index;
extern char* m_buffer;
extern char* m_content;

#define _peek(...) _peek_impl ## __VA_OPT__(1)(__VA_ARGS__)
#define _peek_impl1(...) peek(__VA_ARGS__)
#define _peek_impl(...) peek(0)

#define _consume(...) _consume_impl ## __VA_OPT__(1)(__VA_ARGS__)
#define _consume_impl1(...) consume(__VA_ARGS__)
#define _consume_impl(...) consume(1)

#endif // PARSER_HPP
