#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "postings.h"

int compare_v(post_entry* first, post_entry* second)
{
    int ret = strcmp(first->word, second->word);

    if (ret == 0) 
    {
        return (int) ((1000000 * first->weight) - (1000000 * second->weight)) * -1; 
    } else return ret;
}

int compare(const void* l, const void* r)
{
    post_entry* first = (post_entry*)l;
    post_entry* second = (post_entry*)r;


    int ret = strcmp(first->word, second->word);

    if (ret == 0) 
    {
        return (int) ((1000000 * second->weight) - (1000000 * first->weight)); 
    } else return ret;
}

void swap(post_entry* first, post_entry* second)
{
    post_entry temp;

    memcpy(temp.word, first->word, MAX_WORD_LENGTH);
    memcpy(temp.file_name, first->file_name, MAX_FILE_LENGTH);
    temp.weight = first->weight;

    memcpy(first->word, second->word, MAX_WORD_LENGTH);
    memcpy(first->file_name, second->file_name, MAX_FILE_LENGTH);
    first->weight = second->weight;

    memcpy(second->word, temp.word, MAX_WORD_LENGTH);
    memcpy(second->file_name, temp.file_name, MAX_FILE_LENGTH);
    second->weight = temp.weight;
}

void merge_sort(post_entry* list, uint64_t start, uint64_t end)
{
    if (start == end) return;

    uint64_t mid = start + (end - start) / 2;

    merge_sort(list, start, mid);
    merge_sort(list, mid + 1, end);

    uint64_t i = start;
    uint64_t j = mid;

    while (i != end && j != end)
    {
        
    }
}

void postings_init(postings* postings)
{
    postings->_max_size = STARTING_SIZE;
    postings->_cursor = 0;
    postings->_sorted = false;
    postings->size = 0;
    postings->list = calloc(postings->_max_size, sizeof(post_entry));
}

void postings_load(postings* post, const char* filepath)
{
    FILE* infile = fopen(filepath, "r");
    if (infile == NULL) {return;}

    char* str = NULL;

    size_t len = 0;

    getline (&str, &len, infile);
    str[strlen(str) - 1] = '\0';
    //printf("%s\n", str);
    post->size = atoi(str);
    
    post->list = calloc(post->size, sizeof(post_entry));

    for (uint i = 0; i < post->size && infile != NULL; ++i)
    {
        uint index = 0;
        char* list[3];
        getline (&str, &len, infile);
        char *ptr = strtok(str, DELIMINATER);

        while(ptr != NULL)
        {
            //printf("'%s'\n", ptr);
            list[index++] = ptr;
            ptr = strtok(NULL, DELIMINATER);
        }
        
        //printf("%s\n", list[0]);
        memcpy(post->list[i].word, list[0], MAX_WORD_LENGTH);
        memcpy(post->list[i].file_name, list[1], MAX_WORD_LENGTH);
        
        list[2][strlen(list[2]) - 1] = '\0';
        post->list[i].weight = atof(list[2]);
    }
}

void postings_delete(postings* postings)
{
    free(postings->list);
    memset(postings, 0, sizeof(*postings));
}

void postings_add(postings* postings, const char* key, const char* filename, float_t weight)
{
    if (postings->_max_size - 1 == postings->size)
    {
        postings->_max_size = postings->_max_size * 2;
        post_entry* newlist = calloc(postings->_max_size, sizeof(post_entry));

        for (uint64_t i = 0; i <= postings->size; ++i)
        {
            memcpy(newlist[i].word, postings->list[i].word, MAX_WORD_LENGTH);
            memcpy(newlist[i].file_name, postings->list[i].file_name, MAX_FILE_LENGTH);
            newlist[i].weight = postings->list[i].weight;
        }

        free(postings->list);
        postings->list = newlist;
    }

    memcpy(postings->list[postings->_cursor].word, key, MAX_WORD_LENGTH);
    memcpy(postings->list[postings->_cursor].file_name, filename, MAX_FILE_LENGTH);
    postings->list[postings->_cursor].weight = weight;

    ++postings->_cursor;
    ++postings->size;
}

void postings_sort(postings* postings)
{
    //merge_sort(postings->list, 0, postings->size);
    qsort(postings->list, postings->size, sizeof(post_entry), compare);
}

void postings_printto_file(postings* postings, const char* filepath)
{

    FILE* outfile = fopen(filepath, "w+");
    if (outfile == NULL || outfile == 0) {return;}

    fprintf(outfile, "%lu\n", postings->size);

    for (uint64_t i = 0; i < postings->size; ++i)
    {
        fprintf(outfile, "%s:%s:%f\n", postings->list[i].word, postings->list[i].file_name, postings->list[i].weight);
        //fprintf(outfile, "%s:%f\n", postings->list[i].file_name, postings->list[i].weight);
    }
    fclose(outfile);
}
