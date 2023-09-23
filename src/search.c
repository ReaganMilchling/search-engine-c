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

void calc_corpus(const char* dir)
{
    if (strlen(dir) < 2) return;
    printf("File directory: %s\n", dir);
    clock_t begin = clock();

    u_int count = 503; 
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
    
    printf("%lu\n", dict.size);

    for (uint i = 0; i < len; ++i)
    {
        dict_entry* entry;
        entry = dictionary_find(&dict, list[i]);
        if (entry != NULL)
        {
            printf("%s\n", entry->word);
            dict_entry_print(entry);
        }
    }
}

int main(int argc, char *argv[])
{
    char directory[MAX_FILENAME_LEN] = { "/home/reagan/development/search_engine_c/resources/HTMLFiles" };

    //if (argc == 1) {return -1;}
    if (argc <= 2) 
    {
        char* arr[] = {"computer", "science", "accuracy"};
        search(arr, 3);
        //calc_corpus(directory);
    }
    else if (strcmp(argv[1], "init") == 0)
    {
        calc_corpus(argv[2]);
    }
    else if (strcmp(argv[1], "search") == 0)
    {
        search(&argv[2], argc-2);
    }

    return(0);
}


