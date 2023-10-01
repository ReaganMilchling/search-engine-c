#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arraylist.h"

#define INITIAL_LIST_SIZE 1024
#define GROWTH_FACTOR 2

struct arraylist {
    void *list;
    size_t size;

    bool sorted;
    size_t max_size;
    size_t item_size;
    cb_cmp fn_cmp;
    cb_stream fn_stream;
};

arraylist* list_init(size_t size, cb_cmp fn_cmp, cb_stream fn_stream)
{
    if (fn_cmp == NULL || fn_stream == NULL || size == 0)
        return 0;

    arraylist* l = calloc(1, sizeof(arraylist));

    l->fn_cmp = fn_cmp;
    l->fn_stream = fn_stream;
    l->sorted = false;
    l->max_size = INITIAL_LIST_SIZE;
    l->item_size = size;

    l->size = 0;
    l->list = calloc(l->max_size, l->item_size);

    return l;
}

void list_load(arraylist* this, const char* filepath, cb_load fn_load, bool sorted)
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
    this->list = calloc(this->size, this->item_size);

    for (uint i = 0; i < this->size && infile != NULL; ++i)
    {
        getline (&str, &len, infile);
        fn_load(this->list + (this->item_size) * i, str);
    }

    if (!sorted) 
        list_sort(this);

    this->sorted = true;
}

void list_destroy(arraylist* this)
{
    free(this->list);
    free(this);
}

void list_append(arraylist* this, void *value)
{
    list_insert(this, value, this->size);
}

void list_insert(arraylist* this, void *value, size_t index)
{
    if (index > this->size)
        index = this->size;

    if (this->max_size == this->size)
    {
        this->max_size *= GROWTH_FACTOR;
        this->list = realloc(this->list, this->item_size*this->max_size);
    }
    
    if (index < this->size)
    {
        memmove(this->list + (this->item_size) * (index + 1), 
                this->list + (this->item_size) * (index),
                this->item_size);
    }

    memcpy(this->list + (this->item_size) * index, value, this->item_size);

    ++this->size;
    this->sorted = false;
}

void list_sort(arraylist* this)
{
    if (!this->sorted)
    {
        qsort(this->list, this->size, this->item_size, this->fn_cmp); 
        this->sorted = true;
    }
}

void *list_get(arraylist* this, size_t index)
{
    if (index > this->size) index = this->size - 1;

    return this->list + (this->item_size) * (index);
}

void *list_find(arraylist* this, void *value)
{
    if (this->sorted) 
    {
        return bsearch(value, this->list, this->size, this->item_size, this->fn_cmp);
    } 
    else 
    {
        for (uint i = 0; i < this->size; ++i)
        {
            if (this->fn_cmp(this->list + (this->item_size) * i, value) == 0) 
            {
                return (this->list + (this->item_size) * i);
            }
        }
    }
    return 0;
}

void list_print_file(arraylist* this, const char* filepath)
{
    FILE* outfile = fopen(filepath, "w+");
    if (outfile == NULL || outfile == 0) 
        return;

    fprintf(outfile, "%lu\n", this->size);

    for (uint64_t i = 0; i < this->size; ++i)
    {
        this->fn_stream(this->list + (this->item_size) * i, outfile);
    }
    fclose(outfile);
}

void list_print_console(arraylist* this, size_t index)
{
    if (index >= this->size)
        index = this->size - 1;

    this->fn_stream(this->list + (this->item_size) * index, stdout);
}
