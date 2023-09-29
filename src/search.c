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
#include "arraylist/arraylist.h"
#include "dictionary/dictionary.h"
#include "postings/postings.h"


int compare_dict_entry(const void* l, const void* r)
{
    dict_entry* first = (dict_entry*)l;
    dict_entry* second = (dict_entry*)r;
    printf("%s:%s\n", first->word, second->word);
    return strcmp(first->word, second->word);
};

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
    fprintf(stream, "%s:%hu:%lu\n", data->word, data->length, data->index_start);
};

int compare_post_entry(const void* l, const void* r)
{
    post_entry* first = (post_entry*)l;
    post_entry* second = (post_entry*)r;

    int ret = strcmp(first->word, second->word);

    if (ret == 0) 
    {
        return (int) ((second->weight - first->weight)*1000000); 
    } else return ret;
};

void post_write(const void *d, char *str)
{
    post_entry* data = (post_entry*)d;

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
    memcpy(data->file_name, list[1], MAX_WORD_LENGTH);

    list[2][strlen(list[2]) - 1] = '\0';
    data->weight = atof(list[2]);
}

void post_entry_stream(const void *d, FILE* stream)
{
    post_entry* data = (post_entry*)d;
    fprintf(stream, "%s:%s:%f\n", data->word, data->file_name, data->weight);
};

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
    ArrayList postings;
    list_init(&postings, sizeof(post_entry), compare_post_entry, post_entry_stream);
    list_load(&postings, "../output/post.txt", post_write);
    list_print_file(&postings, "../output/postteststts.txt");

    ArrayList dictl;
    list_init(&dictl, sizeof(dict_entry), compare_dict_entry, dict_entry_stream);
    list_load(&dictl, "../output/dict.txt", dict_write);
    list_print_file(&dictl, "../output/dicttest1.txt");

    hashmap scores;
    hashmap_create(&scores, DEFAULT_HASHMAP_SIZE);

    for (uint i = 0; i < len; ++i)
    {
        dict_entry *search = calloc(1, sizeof(dict_entry));
        //memcpy(search->word, list[i], MAX_WORD_LENGTH);
        strncpy(search->word, list[i], 128);



        dict_entry* entry = (dict_entry*) list_find(&dictl, search);
        //entry = dictionary_find(&dict, list[i]);
        if (entry != NULL)
        {
            dict_entry_print(entry);
            for (uint i = entry->index_start; i < entry->index_start + entry->length; ++i)
            {
                //printf("%s ", post.list[i].word);
                //printf("File: %s Score: %f\n", post.list[i].file_name, post.list[i].weight);
                post_entry *tmp = (post_entry*)list_get(&postings, i);
                hashmap_entry* entry;
                entry = hashmap_insert(&scores, tmp->file_name);
                entry->frequency += (int)(tmp->weight * 1000000);
            }
        }
        free(search);
    }
    hashmap_printto_file(&scores, "../output/scores.txt");
    qsort(scores.table, scores.max_size, sizeof(hashmap_entry), compare_hash);
    
    for (uint i = 0; i < 10 && i <scores.curr_size; ++i)
    {
        printf("%u match: %s with score: %f\n", i, scores.table[i].word, ((float)scores.table[i].frequency)/1000000);
    }

    //list_destroy(&test);
    //dictionary_delete(&dict);
}

int main(int argc, char *argv[])
{
    char directory[MAX_FILENAME_LEN] = { "/home/reagan/development/search_engine_c/resources/HTMLFiles" };
    char* arr[] = {"godot", "unity", "industry", "shareholders"};
    char* arr2[] = {"godot"};

    if (argc < 2) 
    {
        search(arr2, 1);
    } 
    else if (strcmp(argv[1], "init") == 0)
    {
        if (argc < 3) {
            calc_corpus(directory);
        } else {
            calc_corpus(argv[2]);
        }
    }
    else if (strcmp(argv[1], "search") == 0)
    {
        if (argc < 3) {
            search(arr, 4);
        } else {
            search(&argv[2], argc-2);
        }
    }

    return(0);
}


