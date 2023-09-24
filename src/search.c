#include <ctype.h>
#include <dirent.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "objects.h"
#include "trie/trie.h"
#include "hashmap/hashmap.h"
#include "dictionary/dictionary.h"
#include "postings/postings.h"



int compare_hash(const void* l, const void* r)
{
    hashmap_entry* first = (hashmap_entry*)l;
    hashmap_entry* second = (hashmap_entry*)r;

    return second->frequency - first->frequency; 
};

void calc_corpus(const char* dir)
{
    if (strlen(dir) < 2) return;
    printf("File directory: %s\n", dir);
    clock_t begin = clock();

    u_int count = 506; 
    corpus corpus;
    corpus_init(&corpus, count);
    process_corpus(&corpus, dir, count);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time spent: %f\n", time_spent);

    corpus_delete(&corpus, count);
}

void search(char* list[], int len)
{
    postings post;
    dictionary dict;
    postings_load(&post, "../output/post.txt");
    dictionary_loadfile(&dict, "../output/dict.txt");
    dictionary_printto_file(&dict, "../output/testing.txt");
    
    hashmap scores;
    hashmap_create(&scores, DEFAULT_HASHMAP_SIZE);

    printf("%lu\n", dict.size);

    for (uint i = 0; i < len; ++i)
    {
        dict_entry* entry;
        entry = dictionary_find(&dict, list[i]);
        if (entry != NULL)
        {
            dict_entry_print(entry);
            for (uint i = entry->index_start; i < entry->index_start + entry->length; ++i)
            {
                //printf("%s ", post.list[i].word);
                //printf("File: %s Score: %f\n", post.list[i].file_name, post.list[i].weight);
                hashmap_entry* entry;
                entry = hashmap_insert(&scores, post.list[i].file_name);
                entry->frequency += (int)(post.list[i].weight * 1000000);
            }
        }
    }
    hashmap_printto_file(&scores, "../output/scores.txt");
    qsort(scores.table, scores.max_size, sizeof(hashmap_entry), compare_hash);
    
    for (uint i = 0; i < 10 && i <scores.curr_size; ++i)
    {
        printf("%u match: %s with score: %f\n", i, scores.table[i].word, ((float)scores.table[i].frequency)/1000000);
    }
}

int main(int argc, char *argv[])
{
    char directory[MAX_FILENAME_LEN] = { "/home/reagan/development/search_engine_c/resources/HTMLFiles" };

    if (argc == 1) {return -1;}
    if (strcmp(argv[1], "init") == 0)
    {
        if (argc < 3)
        {
            calc_corpus(directory);
        } else {
            calc_corpus(argv[2]);
        }
    }
    else if (strcmp(argv[1], "search") == 0 && argc >= 3)
    {
        search(&argv[2], argc-2);
    }
    else if (argc <= 2) 
    {
        char* arr[] = {"computer", "unity", "industry", "shareholders"};
        search(arr, 4);
        //calc_corpus(directory);
    }

    return(0);
}


