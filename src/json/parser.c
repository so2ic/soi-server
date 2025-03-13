#include "parser.h"

// Define the global variables
int m_index = 0;
char* m_buffer = NULL;
char* m_content = NULL;

// Parsing function
card_t* json_to_cards(const char* file_path, card_t* card)
{
    FILE* file;
    long length;
    optionnal_char_t c;
    m_index = 0;
    size_t deck_size = 0;

    file = fopen(file_path, "rb");
    if (file == NULL)
    {
        perror("Error reading file");
        return NULL; // Handle error
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
        return NULL; // Handle memory allocation failure
    }

    fclose(file);

    while((c = _peek()).has_value)
    {
        // Skip the first { at start
        if(c.value == '{' && m_index == 0)
        {
            _consume();
            continue;
        }

        // skip the last {
        if(c.value == '}' && m_index == strlen(m_content - 1))
        {
            _consume();
           continue; 
        }

        if(c.value == ' ' || c.value == '\n')
        {
            _consume();
            continue; 
        }

        // Start of card creation
        // Need improvement later on
        if(c.value == '{')
        {
            deck_size++;
            card = (card_t*) realloc(card, deck_size * sizeof(card_t));
            strcpy(card[deck_size-1].name,  m_buffer);
            free(m_buffer);
            m_buffer = NULL;
        }

        // Start of the field
        if(c.value == '"')
            _consume();

        int field_length = 0;
        while((c = _peek(field_length)).has_value)
        {
            // End of the field
            if(c.value == '"')
            {
                break;
            }
            ++field_length;
        }
        
        // Allocate m_buffer and copy field
        m_buffer = _consume(field_length);

        // to escape end of field
        _consume();
        if(!m_buffer)
        {
           printf("ERROR"); 
           break; // to handle properly
        }

        printf("%s\n", m_buffer);

        free(m_buffer);
        m_buffer = NULL;
    }

    printf("File read\n");

    free(m_content);
    m_content = NULL;

    return NULL;
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
