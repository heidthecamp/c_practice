#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// structures
struct dictionary_node {
    bool isWord;
    struct dictionary_node *children[26];
};

// global variable containing the dictionary.
char** _dictionary_list = NULL;
int _dictionary_length = 0;
int _dictionary_capacity = 0;

struct dictionary_node *_dictionary_head = NULL;

// allocate new memory
bool dictionary_init() 
{
    dictionary_free();
    // load in dictionary file
    FILE *fp = fopen("words_alpha.txt", "rt");

    if (!fp) {
        return false;
    }

    _dictionary_head = malloc(sizeof(struct dictionary_node));
    _dictionary_head->isWord = false;
    memset(_dictionary_head->children, 0, sizeof(_dictionary_head->children));

    _dictionary_capacity = 1024;
    _dictionary_list = malloc(_dictionary_capacity * sizeof(char*));

    char* word = NULL;
    size_t word_size = 0;
    while (getline(&word, &word_size, fp) != -1) {
        // if capacity full
        // realloc(pointer, newsized) //May return same pointer.
        if (_dictionary_length == _dictionary_capacity) {
            _dictionary_capacity *= 2;
            _dictionary_list = realloc(_dictionary_list, _dictionary_capacity * sizeof(char*));
        }

        size_t len = strlen(word);

        if (word[len - 1] == '\n') {
            word[len - 1] = '\0';
            --len;
        }

        if (word[len - 1] == '\r') {
            word[len - 1] = '\0';
            --len;
        }

        // strdup()
        _dictionary_list[_dictionary_length] = strdup(word);

        ++_dictionary_length;

        struct dictionary_node *ptr = _dictionary_head;
        for(int i = 0; i < len; ++i) {
            int offset = word[i] - 'a';
            if (!ptr->children[offset]) {
                struct dictionary_node *child = malloc(sizeof(struct dictionary_node));
                child->isWord = false;
                memset(child->children, 0, sizeof(child->children));
                ptr->children[offset] = child;
            }
            ptr = ptr->children[offset];
        }
        ptr->isWord = true;
    }

    free(word);
    fclose(fp);
    return true;
}

void _free_node(struct dictionary_node *node)
{
    if (!node) {
        return;
    }
    for (int i = 0; i < 26; ++i) {
        _free_node(node->children[i]);
    }
    free(node);
}

// free allocated memory
void dictionary_free()
{
    _free_node(_dictionary_head);

    for(int i = 0; i < _dictionary_length; ++i) {
        free(_dictionary_list[i]);
    }
    free(_dictionary_list);
}

// see if a word exists and return result
// bool dictionary_lookup(const char * word)
// {
//     for (int i = 0; i < _dictionary_length; ++i) {
//         if (strcmp(word, _dictionary_list[i]) == 0){
//             return true;
//         }
//     }
//     return false;
// }

bool dictionary_lookup(const char * word)
{
    size_t len = strlen(word);

    struct dictionary_node *ptr = _dictionary_head;
    for (size_t i = 0; i < len; ++i) {
        int offset = word[i] - 'a';
        if (!ptr->children[offset]) {
            return false;
        }
        ptr = ptr->children[offset];
    }

    return ptr->isWord;
}

// get the number of words in the dictionary
size_t dictionary_word_count()
{
    return _dictionary_length;
}

// return a word from the wordlist at an index
char * dictionary_get_word(size_t index)
{
    return _dictionary_list[index];
}