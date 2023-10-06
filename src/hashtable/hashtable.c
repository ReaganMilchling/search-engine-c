#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

#define INITIAL_TABLE_SIZE 128
#define GROWTH_FACTOR 2


struct hashtable {
    void *list;
    size_t max_size;
    size_t size;
    
    size_t key_size;
    size_t value_size;
    size_t bucket_size;

    cb_ht_hash fn_hash; 
    cb_ht_insert fn_insert;
    cb_ht_stream fn_stream;
};

size_t hash_string(void *d)
{
    char *str = (char *)d;
    //http://www.cse.yorku.ca/~oz/hash.html - djb2
    size_t hash = 5381;
    size_t c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

hashtable* ht_create(size_t key_size, size_t val_size, cb_ht_hash fn_hash, cb_ht_insert fn_insert, cb_ht_stream fn_stream)
{
    if (fn_stream == NULL || key_size < 1 || val_size < 1)
        return NULL;

    hashtable *t = calloc(1, sizeof(hashtable));
    
    t->key_size = key_size;
    t->value_size = val_size;
    t->bucket_size = t->key_size + t->value_size;
    t->max_size = INITIAL_TABLE_SIZE;

    if (fn_hash == NULL) {
        t->fn_hash = hash_string;
    } else {
        t->fn_hash = fn_hash;
    }
    t->fn_insert = fn_insert;
    t->fn_stream = fn_stream;

    t->size = 0;
    //printf("k:%zu - v:%zu\n",t->key_size, t->value_size);
    //printf("ALLOC:%zu:%zu\n",t->max_size, t->bucket_size);
    t->list = calloc(t->max_size, t->bucket_size);

    return t;
}

void ht_load(hashtable *this, const char* filepath, cb_ht_load fn_load)
{
    // The first line must always be an integer that holds the size of the list (line numbers)
    FILE* infile = fopen(filepath, "r");
    if (infile == NULL) 
        return;
    
    if (this->size != 0)
        free(this->list);

    char* str = NULL;
    size_t len = 0;

    getline (&str, &len, infile);
    str[strlen(str) - 1] = '\0';
    this->size = atoi(str);
    this->list = calloc(this->size, this->bucket_size);

    for (uint i = 0; i < this->size && infile != NULL; ++i)
    {
        getline (&str, &len, infile);
        void *key, *value;
        fn_load(key, value, str);
        ht_insert(this, key, value);
    }
}

void ht_delete(hashtable *this)
{
    free(this->list);
    //free(this);
}

bool ht_exists(hashtable *this, void* key)
{
    return ht_find(this, key) == NULL;
}

void *ht_find(hashtable *this, void* key)
{
    size_t index = this->fn_hash(key) % this->max_size;

    while(*(char *)(this->list + this->bucket_size * index) != 0)
    {
        if (memcmp(key, this->list + this->bucket_size * index, this->key_size) == 0) 
        {
            return this->list + this->bucket_size * index;
        }
        index++;
    }

    return NULL;
}

void ht_insert(hashtable *this, void* key, void* value)
{
    if (this->size * 2 >= this->max_size)
    {
        void *temp = this->list;
        size_t curr_max = this->max_size;

        this->max_size *= GROWTH_FACTOR;
        this->size = 0;
        this->list = calloc(this->max_size, this->bucket_size);

        for (size_t i = 0; i < curr_max; ++i)
        {
            //if key is null, the first char will always be zero
            if (*(char *)(temp + i * this->bucket_size) != 0)
            {
                ht_insert(this, (temp + i * this->bucket_size), (temp + i * this->bucket_size + this->key_size));
            }
        }
        free(temp);
        printf("s");
    }

    size_t index = this->fn_hash(key) % this->max_size;

    while(*(char *)(this->list + this->bucket_size * index) != 0)
    {
        if (this->fn_hash != NULL && memcmp(key, this->list + this->bucket_size * index, this->key_size) == 0) 
        {
            break;
        }
        index++;
        if (index >= this->max_size)
            index = 0;
    }

    memcpy(this->list + this->bucket_size * index, key, this->key_size);
    memcpy(this->list + this->bucket_size * index + this->key_size, value, this->value_size);
    this->size++;
}

size_t ht_get_item_size(hashtable *table)
{
    return table->bucket_size;
}

size_t ht_get_max_length(hashtable *table)
{
    return table->max_size;
}

void *ht_get_list(hashtable *this)
{
    return this->list;
}

void ht_print_file(hashtable *this, const char* filepath)
{
    FILE* outfile = fopen(filepath, "w+");
    if (outfile == NULL || outfile == 0) {return;}

    fprintf(outfile, "%lu\n", this->size);

    for (size_t i = 0; i < this->max_size; ++i)
    {
        if (*(char *)(this->list + this->bucket_size * i) != 0)
        {
            this->fn_stream(this->list + this->bucket_size * i, this->list + this->bucket_size * i + this->key_size, outfile);
        }
    }
    fclose(outfile);

}

void ht_print_console(hashtable *this, void *key)
{
    void *item = ht_find(this, key);

    this->fn_stream(item, item+this->key_size, stdout);
}
