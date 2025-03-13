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

        if(c.value == ' ' || c.value == '\n')
        {
            _consume();
            continue; 
        }

        // Start of the card name
        if(c.value == '"')
            _consume();

        int name_length = 0;
        while((c = _peek(name_length)).has_value)
        {
            // End of the card name
            if(c.value == '"')
            {
                break;
            }
            ++name_length;
        }
        
        // Allocate m_buffer and copy name
        m_buffer = _consume(name_length);

        // to escape end card name '"' char
        _consume();
        if(!m_buffer)
        {
           printf("ERROR"); 
           break; // to handle properly
        }

        printf("%s\n", m_buffer);

        // Free m_buffer to avoid memory leaks
        free(m_buffer);
        m_buffer = NULL;

        break; // Remove this break to continue parsing
    }

    printf("File read\n");

    // Free m_content to avoid memory leaks
    free(m_content);
    m_content = NULL;

    return NULL;
}

optionnal_char_t peek(int i)
{
    if(m_index + i >= strlen(m_content)) // Use >= instead of >
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
