#ifndef POSTINGS_H
#define POSTINGS_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define STARTING_SIZE 1024
#define MAX_WORD_LENGTH 128
#define MAX_FILE_LENGTH 256
#define DELIMINATER ":"


typedef struct post_entry post_entry;
struct post_entry {
    char file_name[MAX_FILE_LENGTH];
    char word[MAX_WORD_LENGTH];
    float_t weight;
};

typedef struct postings postings;
struct postings {
    uint64_t size;
    post_entry* list;

    bool _sorted;
    uint64_t _max_size;
    uint64_t _cursor;
};

void postings_init(postings* postings);
void postings_load(postings* postings, const char* filepath);
void postings_delete(postings* postings);
void postings_add(postings* postings, const char* key, const char* filename, float_t weight);
void postings_sort(postings* postings);
void postings_printto_file(postings* postings, const char* filepath);

#endif /* POSTINGS_H */
