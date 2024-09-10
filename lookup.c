#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "dictionary.h"

bool prompt(char * prompt, char ** line, size_t * size)
{
    printf("%s", prompt);
    int read = getline(line, size, stdin);
    return (read != -1);
}

int main(int argc, char ** argv)
{
    char * line = NULL;
    size_t line_size = 0;

    if (!dictionary_init()) {
        dictionary_free();
        return 1;
    }

    printf("Type Ctrl+D to exit.\n\n");
    while (prompt("Lookup: ", &line, &line_size)) {
        
        // Trim trailing newline
        size_t length = strlen(line);
        if (line[length - 1] == '\n') {
            line[length - 1] = '\0';
            --length;
        }

        bool is_invalid = false;
        for (char * pch = line; *pch != '\0'; ++pch) {

            // Convert all letters to lowercase
            *pch = tolower(*pch);

            if (!isalpha(*pch)) {
                is_invalid = true;
            }
        }

        // Throw an error if characters other than a-z are present
        if (is_invalid) {
            printf("%s contains invalid characters\n", line);
            continue;
        }

        // Performs a lookup into our dictionary
        bool is_word = dictionary_lookup(line);
        if (is_word) {
            printf("%s is a word\n", line);
        }
        else {
            printf("%s is not a word\n", line);
        }
    }

    free(line);

    dictionary_free();

    return 0;
}