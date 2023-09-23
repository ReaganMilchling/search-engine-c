#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"

#define DELIMINATER ":"


void dictionary_init(dictionary* dict, postings* posting, uint64_t size)
{
    dict->size = size;
    dict->list = calloc(dict->size, sizeof(dict_entry));

    char previous[MAX_WORD_LENGTH] = {0};
    uint64_t index = 0;

    memcpy(previous, posting->list[0].word, MAX_WORD_LENGTH);
    memcpy(dict->list[0].word, previous, MAX_WORD_LENGTH);
    dict->list[0].length = 1;
    dict->list[0].index_start = 0;

    for (uint64_t i = 1; i < posting->size; ++i) 
    {
        if (strcmp(previous, posting->list[i].word) != 0)
        {
            ++index;
            memcpy(dict->list[index].word, posting->list[i].word, MAX_WORD_LENGTH);
            dict->list[index].length = 1;
            dict->list[index].index_start = i;
        } else {
            ++dict->list[index].length;
        }
        memcpy(previous, posting->list[i].word, MAX_WORD_LENGTH);
    }
}

void dictionary_loadfile(dictionary* dict, const char* filepath)
{
    FILE* infile = fopen(filepath, "r");
    if (infile == NULL) {return;}

    char* str = NULL;

    size_t len = 0;
    char* list[3];
    uint index = 0;

    getline (&str, &len, infile);
    str[strlen(str) - 1] = '\0';
    //printf("%s\n", str);
    dict->size = atoi(str);
    
    dict->list = calloc(dict->size, sizeof(dict_entry));

    for (uint i = 0; i < dict->size && infile != NULL; ++i)
    {
        getline (&str, &len, infile);
        char *ptr = strtok(str, DELIMINATER);

        while(ptr != NULL)
        {
            //printf("'%s'\n", ptr);
            list[index++] = ptr;
            ptr = strtok(NULL, DELIMINATER);
        }
        
        //printf("%s\n", list[0]);
        memcpy(dict->list[i].word, list[0], MAX_WORD_LENGTH);
        dict->list[i].length = atoi(list[1]);

        list[2][strlen(list[2]) - 1] = '\0';
        dict->list[i].index_start = atoi(list[2]);
        index = 0;
    }
    //printf("loaded: %s\n", filepath);
    //printf("size: %lu\n", dict->size);
}

void dictionary_delete(dictionary* dictionary)
{
    free(dictionary->list);
    memset(dictionary, 0, sizeof(*dictionary));
}


int compare_dict(const void* l, const void* r)
{
    dict_entry* first = (dict_entry*)l;
    dict_entry* second = (dict_entry*)r;

    return strcmp(first->word, second->word);
}

dict_entry* dictionary_find(dictionary* dict, const char* key)
{
    char word[MAX_WORD_LENGTH] = {0};
    strcpy(word, key);

    return (dict_entry*) bsearch(word, dict->list, dict->size, sizeof(dict_entry), compare_dict);
}

void dict_entry_print(dict_entry* entry)
{
    printf("%s:%hu:%lu\n", entry->word, entry->length, entry->index_start);
}

void dictionary_printto_file(dictionary* dict, const char* filepath)
{
    FILE* outfile = fopen(filepath, "w+");
    if (outfile == NULL || outfile == 0) {return;}

    fprintf(outfile, "%lu\n", dict->size);
    for (uint64_t i = 0; i < dict->size; ++i)
    {
        fprintf(outfile, "%s:%hu:%lu\n", dict->list[i].word, dict->list[i].length, dict->list[i].index_start);
    }
    fclose(outfile);
}
