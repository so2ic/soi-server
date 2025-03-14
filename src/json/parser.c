#include "parser.h"

// Define the global variables
int m_index = 0;
char* m_buffer = NULL;
char* m_content = NULL;

// Parsing function
int json_to_cards(const char* file_path, card_t* card)
{
    FILE* file;
    long length;
    optionnal_char_t c;
    m_index = 0;
    size_t deck_size = 0;
    char* last_token;
    int is_effect = 0;

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

            if(strcmp(last_token,"id") == 0)
                    card->id = atoi(m_buffer);
            if(strcmp(last_token,"mana") == 0)
                    card->mana = atoi(m_buffer);
            if(strcmp(last_token,"power") == 0)
                    card->power = atoi(m_buffer);
            if(strcmp(last_token,"draw") == 0)
                    card->draw = atoi(m_buffer);

            free(m_buffer);
            free(last_token);
            m_buffer = NULL;
            last_token = NULL;
        }
        switch(c.value)
        {
            case ' ':
                _consume();
                break;
            case '\n':
                _consume();
                break;
            case '{':
               if (m_index == 0)
               {
                    _consume();
               } 
               else 
               {
                   if(is_effect)
                   {
                        _consume();
                   }
                   else 
                   {
                        // Maybe uncomment later on
                        //card = (card_t*) malloc(sizeof(card_t));
                        card->name = malloc(strlen(last_token) + 1);
                        strcpy(card->name, last_token);
                        free(last_token);
                        last_token = NULL;
                        _consume();
                        printf("NAME : %s\n", card->name);
                   }
               }
               break;
            case '}':
               _consume();
               break;
            case '"':
               // is m_buffer isn't null, then this is a closing 
               if(m_buffer != NULL) 
               {
                    if(_peek(1).value == ':')
                    {
                        last_token = malloc(strlen(m_buffer + 1));
                        strcpy(last_token, m_buffer);
                        free(m_buffer);
                        m_buffer = NULL;
                        _consume(2);
                        printf("TOKEN : %s\n", last_token);
                    }
                    else
                    {
                        if(strcmp(last_token, "class") == 0)
                        {
                            if(strcmp(m_buffer, "allies") == 0)
                              card->type = (CARD_TYPE) ALLIES;  

                            if(strcmp(m_buffer, "mercenaries") == 0)
                              card->type = (CARD_TYPE) ALLIES;  

                            if(strcmp(m_buffer, "champion") == 0)
                              card->type = (CARD_TYPE) ALLIES;  
                        }

                        if(strcmp(last_token, "type") == 0)
                        {
                           if(strcmp(m_buffer, "base") == 0)
                               card->class = (CARD_CLASS) BASE_DECK;
                        }

                        _consume();
                        free(last_token);
                        free(m_buffer);
                        m_buffer = NULL;
                        last_token = NULL;
                    }
               }
               else 
               {
                    _consume();
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
                    printf("BUFFER : %s\n", m_buffer);
               }
               break;
            case '[':
               is_effect = 1;
               _consume();
               break;
            case ']':
               is_effect = 0;
               _consume();
               break;
            case ',':
               _consume();
               break;

            default:
               printf("ERROR SHOULDN'T HAPPEND\n");
               printf("ERROR ON : %d, %c", m_index, c.value);
               break;
        }
    }

    printf("File read\n");

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
