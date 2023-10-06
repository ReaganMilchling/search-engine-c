#include <stdlib.h>
#include <string.h>

#include "query.h"
#include "arraylist/arraylist.h"
#include "hashmap/hashmap.h"
#include "hashtable/hashtable.h"

#define STARTING_SIZE 1024
#define MAX_WORD_LENGTH 128
#define MAX_FILE_LENGTH 256
#define DELIMINATER ":"
#define INITIAL_DICTIONARY_SIZE 1024

typedef struct dict_entry dict_entry;
struct dict_entry {
    char word[MAX_WORD_LENGTH];
    size_t length;
    size_t index_start;
};

typedef struct post_entry post_entry;
struct post_entry {
    char file_name[MAX_FILE_LENGTH];
    char word[MAX_WORD_LENGTH];
    float_t weight;
};

typedef struct score_t score_t;
struct score_t {
    char file[MAX_WORD_LENGTH];
    float_t weight;
};

int score_t_cmp(const void* l, const void* r)
{
    score_t *lhs = (score_t *)l;
    score_t *rhs = (score_t *)r;
    return (int)((rhs->weight - lhs->weight) * 1000000);
}

void score_t_stream(const void *d, FILE* stream)
{
    score_t *data = (score_t *)d;
    fprintf(stream, "%s:%f\n", data->file, data->weight);
}

int compare_dict_entry(const void* l, const void* r)
{
    dict_entry* first = (dict_entry*)l;
    dict_entry* second = (dict_entry*)r;
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
        list[index++] = ptr;
        ptr = strtok(NULL, DELIMINATER);
    }

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

void search(char* list[], int len)
{
    arraylist *postings = list_init(sizeof(post_entry), compare_post_entry, post_entry_stream);
    list_load(postings, "../output/post.txt", post_write, true);

    arraylist *dict = list_init(sizeof(dict_entry), compare_dict_entry, dict_entry_stream);
    list_load(dict, "../output/dict.txt", dict_write, true);

    hashtable *scores = ht_create(sizeof(char) * MAX_WORD_LENGTH, sizeof(float_t), NULL, scores_insert, scores_stream);

    for (size_t i = 0; i < len; ++i)
    {
        //casting char[128] to post_entry, words because word is first item in structu
        dict_entry* entry = (dict_entry*) list_find(dict, list[i]);
        if (entry != NULL)
        {
            //printf("%s:%zu:%zu\n", entry->word, entry->length, entry->index_start);
            for (size_t j = entry->index_start; j < entry->index_start + entry->length; ++j)
            {
                post_entry *tmp = (post_entry*)list_get(postings, j);
                ht_insert(scores, tmp->file_name, &tmp->weight);
            }
        }
    }

    //ht_print_file(scores, "../output/scores.txt");
    //printf("%zu\n", sizeof(score_t));
    arraylist *sorted_scores = list_init(sizeof(score_t), score_t_cmp, score_t_stream);
    list_transfer(sorted_scores, ht_get_list(scores), ht_get_max_length(scores), ht_get_item_size(scores));
    list_print_file(sorted_scores, "../output/scores.txt");
    for (size_t i = 0; i < 10; i++)
    {
        list_print_console(sorted_scores, i);
    }
    
    ht_delete(scores);
    list_destroy(sorted_scores);
    list_destroy(dict);
    list_destroy(postings);
}
