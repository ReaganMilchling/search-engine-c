#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <math.h>
#include <stdint.h>

#include "../postings/postings.h"

#define INITIAL_DICTIONARY_SIZE 1024

typedef struct dict_entry dict_entry;
struct dict_entry {
    char word[MAX_WORD_LENGTH];
    uint16_t length;
    uint64_t index_start;
};

typedef struct dictionary dictionary;
struct dictionary {
    uint64_t size;
    dict_entry* list;
};

void dictionary_init(dictionary* dictionary, postings* posting, uint64_t size);
void dictionary_loadfile(dictionary* dictionary, const char* filepath);
void dictionary_delete(dictionary* dictionary);
dict_entry* dictionary_find(dictionary* dict, const char* key);
void dict_entry_print(dict_entry*);
void dictionary_printto_file(dictionary* dictionary, const char* filepath);

#endif /* DICTIONARY_H */
