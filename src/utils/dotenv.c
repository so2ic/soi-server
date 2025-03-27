#include "dotenv.h"

variable_t* variables = NULL;
int md_index = 0;
char* md_content = NULL;
char* md_buffer = NULL;
int variables_count = 0;

int load_env(char* path)
{
    FILE* f;
    long length;
    int var_count = 0;
    char* e = "/.env";
    size_t path_len = strlen(path) + strlen(e) + 1;
    char target[path_len];
    optionnal_char_t c;

    strcpy(target, path);
    strcat(target, e);

    f = fopen(target, "rb");
    if(f == NULL)
    {
        perror("Error reading .env file");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    md_content = malloc(length + 1);
    if(md_content)
    {
        size_t bytes_read = fread(md_content, 1, length, f);
        md_content[bytes_read] = '\0';
    }
    else
    {
        fclose(f);
        return 1;
    }

    fclose(f);

    {
        char* dump = NULL;
        int key_length, value_length;
        while((c = _dot_peek()).has_value)
        {
            if(c.value == '\n')
            {
                dump = _dot_consume();
                free(dump);
                dump = NULL;
                continue;
            }

            var_count++;
            variables = (variable_t*) realloc(variables, var_count * sizeof(variable_t)); 
            key_length = 0;
            value_length = 0;

            while((c = _dot_peek(key_length)).has_value)
            {
                if(c.value == '=')
                    break;
                key_length++;
            }

            variables[var_count - 1].key = malloc(key_length + 1);
            strcpy(variables[var_count - 1].key, _dot_consume(key_length));

            // we consume the '='
            {
                dump = _dot_consume();
                free(dump);
                dump = NULL;
            }

            while((c = _dot_peek(value_length)).has_value)
            {
                if(c.value == '\n')
                    break;

                value_length++;
            }

            variables[var_count - 1].value = malloc(value_length + 1);
            strcpy(variables[var_count - 1].value, _dot_consume(value_length));
        }
    }

    variables_count = var_count;
    return 0;
}

char* get_env(char* key)
{
    for(int i = 0; i < variables_count; ++i)
    {
        if(strcmp(variables[i].key, key) == 0)
            return variables[i].value;
    }

    perror("env key not found");
    exit(errno);
}

optionnal_char_t dot_peek(int i)
{
    if(md_index + i >= strlen(md_content))
        return (optionnal_char_t) {.has_value = 0};
    else
        return (optionnal_char_t) {.value = md_content[md_index + i], .has_value = 1};
}

char* dot_consume(int i)
{
    if(i == 0)
        return "ERROR"; // to handle properly
    char* s = malloc(i + 1);
    for(int j = 0; j < i; ++j)
    {
        s[j] = md_content[md_index];
        ++md_index;
    }

    s[i] = '\0';
    return s;
}
