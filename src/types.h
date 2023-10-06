#ifndef TYPES_H
#define TYPES_H

#include <math.h>
#include <stdio.h>

#define STARTING_SIZE 1024
#define MAX_WORD_LENGTH 128
#define MAX_FILE_LENGTH 256
#define DELIMINATER ":"
#define INITIAL_DICTIONARY_SIZE 1024

typedef struct dict_entry dict_entry;
struct dict_entry {
    char word[MAX_WORD_LENGTH];
    size_t length;
    size_t index_start;
};

typedef struct post_entry post_entry;
struct post_entry {
    char file_name[MAX_FILE_LENGTH];
    char word[MAX_WORD_LENGTH];
    float_t weight;
};

int compare_dict_entry(const void* l, const void* r);
void dict_write(const void *d, char *str);
void dict_entry_stream(const void *d, FILE* stream);

int compare_post_entry(const void* l, const void* r);
void post_write(const void *d, char *str);
void post_entry_stream(const void *d, FILE* stream);

int compare_hash(const void* l, const void* r);
void scores_insert(void* l, void* r);
void scores_stream(const void *k, const void *v, FILE* stream);

#endif //TYPES_H
