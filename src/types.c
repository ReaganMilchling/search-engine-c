#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap/hashmap.h"
#include "types.h"

int compare_dict_entry(const void* l, const void* r)
{
    dict_entry* first = (dict_entry*)l;
    dict_entry* second = (dict_entry*)r;
    //printf("%s:%s\n", first->word, second->word);
    return strcmp(first->word, second->word);
}

void dict_write(const void *d, char *str)
{
    dict_entry* data = (dict_entry*)d;

    size_t index = 0;
    char* list[3];
    char *ptr = strtok(str, DELIMINATER);

    while(ptr != NULL)
    {
        //printf("'%s'\n", ptr);
        list[index++] = ptr;
        ptr = strtok(NULL, DELIMINATER);
    }

    //printf("%s\n", list[0]);
    memcpy(data->word, list[0], MAX_WORD_LENGTH);
    data->length = atoi(list[1]);

    list[2][strlen(list[2]) - 1] = '\0';
    data->index_start = atoi(list[2]);
}

void dict_entry_stream(const void *d, FILE* stream)
{
    dict_entry* data = (dict_entry*)d;
    fprintf(stream, "%s:%zu:%lu\n", data->word, data->length, data->index_start);
}

int compare_post_entry(const void* l, const void* r)
{
    post_entry* first = (post_entry*)l;
    post_entry* second = (post_entry*)r;

    int ret = strcmp(first->word, second->word);

    if (ret == 0) {
        return (int) ((second->weight - first->weight)*1000000); 
    } else return ret;
}

void post_write(const void *d, char *str)
{
    post_entry* data = (post_entry*)d;

    size_t index = 0;
    char* list[3];
    char *ptr = strtok(str, DELIMINATER);

    while(ptr != NULL)
    {
        list[index++] = ptr;
        ptr = strtok(NULL, DELIMINATER);
    }

    memcpy(data->word, list[0], MAX_WORD_LENGTH);
    memcpy(data->file_name, list[1], MAX_WORD_LENGTH);

    list[2][strlen(list[2]) - 1] = '\0';
    data->weight = atof(list[2]);
}

void post_entry_stream(const void *d, FILE* stream)
{
    post_entry* data = (post_entry*)d;
    fprintf(stream, "%s:%s:%f\n", data->word, data->file_name, data->weight);
}

int compare_hash(const void* l, const void* r)
{
    hashmap_entry* first = (hashmap_entry*)l;
    hashmap_entry* second = (hashmap_entry*)r;

    return second->frequency - first->frequency; 
}

void scores_insert(void* l, void* r)
{
    float_t *lhs = (float_t *)l;
    float_t *rhs = (float_t *)r;

    *lhs += *rhs;
}

void scores_stream(const void *k, const void *v, FILE* stream)
{
    char *key = (char *)k;
    float_t* value = (float_t*)v;
    fprintf(stream, "%s:%f\n", key, *value);
}

