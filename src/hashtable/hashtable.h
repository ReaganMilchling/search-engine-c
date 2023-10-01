#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct ht_bucket ht_bucket;
typedef struct hashtable hashtable;

typedef size_t (*cb_hash)(void *key);
typedef void (*cb_insert)(void *l, void*r);
typedef void (*cb_load)(const void *d, char *s);

hashtable* ht_create(size_t key_size, size_t val_size, cb_hash fn_hash, cb_insert fn_add);
void ht_load(const char* filepath, cb_load fn_load);
void ht_delete(hashtable *table);

bool ht_exists(hashtable *table, void* key);
void *ht_find(hashtable *table, void* key);
void *ht_insert(hashtable *table, void* key, void* value);
void *ht_get_list(hashtable *table);

void ht_print(hashtable *table);
void ht_printto_file(hashtable *table, const char* filepath);


#endif /* HASHMAP_H */
