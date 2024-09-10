#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdlib.h>
#include <stdbool.h>

bool dictionary_init();

void dictionary_free();

bool dictionary_lookup(const char * word);

size_t dictionary_word_count();

char * dictionary_get_word(size_t index);

#endif // DICTIONARY_H