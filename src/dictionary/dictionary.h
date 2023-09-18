#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <math.h>
#include <stdint.h>

#define INITIAL_DICTIONARY_SIZE 1024

typedef struct dict_entry dict_entry;
struct dict_entry {
    const char* word;
    uint16_t length;
    uint64_t index_start;
};

typedef struct dictionary dictionary;
struct dictionary {
    uint64_t size;
    dict_entry* list;
};

void dictionary_init(dictionary* dictionary);
void dictionary_delete(dictionary* dictionary);
int dictionary_add(dictionary* dictionary, const char* word, uint16_t length, uint64_t start);
int dictionary_sort(dictionary* dictionary);
int dictionary_print_to_file(dictionary* dictionary, const char* filepath);

#endif /* DICTIONARY_H */
