#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arraylist.h"


void list_init(ArrayList* this, size_t size, callback_cmp fn_cmp, callback_stream fn_stream)
{
    if (fn_cmp == NULL || fn_stream == NULL || size == 0)
        return;

    this->_fn_cmp = fn_cmp;
    this->_fn_stream = fn_stream;
    this->_sorted = false;
    this->_max_size = INITIAL_LIST_SIZE;
    this->_type_byte_size = size;

    this->size = 0;
    this->list = calloc(this->_max_size, this->_type_byte_size);
}

void list_load(ArrayList* this, const char* filepath, callback_load fn_load)
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
    this->list = calloc(this->size, this->_type_byte_size);

    for (uint i = 0; i < this->size && infile != NULL; ++i)
    {
        getline (&str, &len, infile);
        fn_load(this->list + (this->_type_byte_size) * i, str);
    }
    this->_sorted = true;
}

void list_destroy(ArrayList* this)
{
    free(this->list);
    free(this);
}

void list_append(ArrayList* this, void *value)
{
    list_insert(this, value, this->size);
}

void list_insert(ArrayList* this, void *value, size_t index)
{
    if (index > this->size)
        index = this->size;

    if (this->_max_size == this->size)
    {
        this->_max_size *= GROWTH_FACTOR;
        this->list = realloc(this->list, this->_type_byte_size*this->_max_size);
    }
    
    if (index < this->size)
    {
        memmove(this->list + (this->_type_byte_size) * (index + 1), 
                this->list + (this->_type_byte_size) * (index),
                this->_type_byte_size);
    }

    memcpy(this->list + (this->_type_byte_size) * index, value, this->_type_byte_size);

    ++this->size;
    this->_sorted = false;
}

void list_sort(ArrayList* this)
{
    if (!this->_sorted)
    {
        qsort(this->list, this->size, this->_type_byte_size, this->_fn_cmp); 
        this->_sorted = true;
    }
}

void *list_get(ArrayList* this, size_t index)
{
    if (index > this->size) index = this->size - 1;

    return this->list + (this->_type_byte_size) * (index);
}

void *list_find(ArrayList* this, void *value)
{
    if (this->_sorted) 
    {
        return bsearch(value, this->list, this->size, this->_type_byte_size, this->_fn_cmp);
    } 
    else 
    {
        for (uint i = 0; i < this->size; ++i)
        {
            if (this->_fn_cmp(this->list + (this->_type_byte_size) * i, value) == 0) 
            {
                return (this->list + (this->_type_byte_size) * i);
            }
        }
    }
    return 0;
}

void list_print_file(ArrayList* this, const char* filepath)
{
    FILE* outfile = fopen(filepath, "w+");
    if (outfile == NULL || outfile == 0) 
        return;

    fprintf(outfile, "%lu\n", this->size);

    for (uint64_t i = 0; i < this->size; ++i)
    {
        this->_fn_stream(this->list + (this->_type_byte_size) * i, outfile);
    }
    fclose(outfile);
}

void list_print_console(ArrayList* this, size_t index)
{
    if (index >= this->size)
        index = this->size - 1;

    this->_fn_stream(this->list + (this->_type_byte_size) * index, stdout);
}
