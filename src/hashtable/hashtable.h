#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct hashtable;
typedef struct hashtable hashtable;

typedef size_t (*cb_ht_hash)(void *key);
typedef void (*cb_ht_insert)(void *l, void*r);
typedef void (*cb_ht_stream)(const void* k, const void *v, FILE* stream);
typedef void (*cb_ht_load)(const void *k, const void *v, char *s);

hashtable *ht_create(size_t key_size, size_t val_size, cb_ht_hash fn_hash, cb_ht_insert fn_add, cb_ht_stream fn_stream);
void ht_load(hashtable *table, const char* filepath, cb_ht_load fn_load);
void ht_delete(hashtable *table);

bool ht_exists(hashtable *table, void* key);
void ht_insert(hashtable *table, void* key, void* value);
void *ht_find(hashtable *table, void* key);

size_t ht_get_item_size(hashtable *table);
size_t ht_get_max_length(hashtable *table);
void *ht_get_list(hashtable *table);

void ht_print_file(hashtable *table, const char* filepath);
void ht_print_console(hashtable *table, void *key);

#endif /* HASHTABLE_H */
