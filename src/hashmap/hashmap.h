#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_WORD_LENGTH 128

typedef struct hashmap_entry hashmap_entry;
typedef struct hashmap hashmap;

struct hashmap_entry {
    char word[MAX_WORD_LENGTH];
    uint64_t frequency;
};

struct hashmap {
    hashmap_entry *table;
    uint64_t max_size;
    uint64_t curr_size;
};

void hashmap_create(hashmap *map, uint64_t size);
void hashmap_delete(hashmap *map);
bool hashmap_exists(hashmap *map, const char* key);
hashmap_entry* hashmap_find(hashmap *map, const char* key);
hashmap_entry* hashmap_insert(hashmap *map, const char* key);
void hashmap_print(hashmap *map);
void hashmap_printto_file(hashmap *map, const char* filepath);


#endif /* HASHMAP_H */
