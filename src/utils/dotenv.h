#ifndef DOTENV_H
#define DOTENV_H

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef OPT_CHAR
#define OPT_CHAR
typedef struct {
    char value;
    int has_value;
} optionnal_char_t;
#endif

typedef struct
{
    char* key, *value;
} variable_t;

int load_env(char* path);
char* get_env(char* key);

// TODO
// Change all of this by struct
extern variable_t *variables;
extern int md_index;
extern char* md_buffer;
extern char* md_content;
extern int variables_count;

optionnal_char_t dot_peek(int i);
char* dot_consume(int i);

#define _dot_peek(...) _dot_peek_impl ## __VA_OPT__(1)(__VA_ARGS__)
#define _dot_peek_impl1(...) dot_peek(__VA_ARGS__)
#define _dot_peek_impl(...) dot_peek(0)

#define _dot_consume(...) _dot_consume_impl ## __VA_OPT__(1)(__VA_ARGS__)
#define _dot_consume_impl1(...) dot_consume(__VA_ARGS__)
#define _dot_consume_impl(...) dot_consume(1)

#endif // DOTENV_H
