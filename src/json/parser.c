#include "parser.h"

card_t* json_to_cards(const char* file_path)
{
    FILE* file;
    int c;
    char current;

    file = fopen(file_path, "r");
    if(file == NULL)
    {
        perror("Error reading file");
        // Handle error
    }

    while((c = fgetc(file)) != EOF)
    {
        current = (char) c;
        if(current == ' ')
            continue;
        if(current == '\0')
            printf("\n");
        printf("%c", current); 
    }

    printf("File read\n");

    fclose(file);

    return NULL;
}
