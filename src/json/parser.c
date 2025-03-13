#include "parser.h"

card_t* json_to_cards(const char* file_path, card_t* card)
{
    FILE* file;
    char current;
    long length;
    optionnal_char_t c;
    m_index = 0;

    file = fopen(file_path, "rb");
    if(file == NULL)
    {
        perror("Error reading file");
        // Handle error
    }

    // Copy file content into a string
    fseek (file, 0, SEEK_END);
    length = ftell (file);
    fseek (file, 0, SEEK_SET);
    m_content = malloc (length);
    if (m_content)
    {
        fread (m_content, 1, length, file);
    }

    while((c = peek()).has_value)
    {
        printf("%c", consume());
    }

    printf("File read\n");

    fclose(file);

    return NULL;
}

optionnal_char_t peek()
{
    if(m_index > strlen(m_content))
        return (optionnal_char_t) {.has_value = 0};
    else
        return (optionnal_char_t) {.value = m_content[m_index], .has_value = 1};
}

char consume()
{
    int t = m_index;
    ++m_index;
    return m_content[t];
}
