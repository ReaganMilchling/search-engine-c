#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

#define INITIAL_TABLE_SIZE 1024
#define GROWTH_FACTOR 2

struct ht_bucket {
    void *key;
    void *value;
};

struct hashtable {
    ht_bucket *list;
    size_t max_size;
    size_t curr_size;
    
    size_t key_size;
    size_t value_size;

    cb_hash fn_hash; 
    cb_insert fn_insert;
};

hashtable* ht_create(size_t key_size, size_t val_size, cb_hash fn_hash, cb_insert fn_insert)
{
    hashtable *t = calloc(1, sizeof(hashtable));

    t->key_size = key_size;
    t->value_size = val_size;
    t->fn_hash = fn_hash;
    t->fn_insert = fn_insert;
    t->max_size = INITIAL_TABLE_SIZE;

    t->curr_size = 0;
    t->list = calloc(t->max_size, t->key_size + t->value_size);

    return t;
}

void ht_load(const char* filepath, cb_load fn_load)
{

}

void ht_delete(hashtable *this)
{
    free(this->list);
    free(this);
}

bool ht_exists(hashtable *this, void* key)
{
    return ht_find(this, key) == NULL;
}

void *ht_find(hashtable *this, void* key)
{
    return NULL;
}

void *ht_insert(hashtable *this, void* key, void* value)
{
    return NULL;
}

void *ht_get_list(hashtable *this)
{
    return this->list;
}

void ht_print(hashtable *this)
{

}

void ht_printto_file(hashtable *this, const char* filepath)
{

}

