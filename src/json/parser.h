#ifndef PARSER_HPP
#define PARSER_HPP

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "../card.h"

card_t* json_to_cards(const char* file_path);

#endif // PARSER_HPP
