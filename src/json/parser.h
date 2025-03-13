#ifndef PARSER_HPP
#define PARSER_HPP

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "../card.h"

// Struct to represent an optional character
typedef struct {
    char value;
    int has_value;
} optionnal_char_t;

// Function declarations
card_t* json_to_cards(const char* file_path, card_t* card);
optionnal_char_t peek(int i);
char consume();

// Global variables
extern int m_index;
extern char* m_buffer;
extern char* m_content;

#define _peek(...) _peek_impl ## __VA_OPT__(1)(__VA_ARGS__)
#define _peek_impl1(...) peek(__VA_ARGS__)
#define _peek_impl(...) peek(0)

#endif // PARSER_HPP
