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

int process_corpus(corpus* corpus, const char* filepath, u_int count);

int main(int argc, char *argv[])
{
    clock_t begin = clock();

    u_int count = 504; 
    corpus corpus;
    corpus_init(&corpus, count);
    process_corpus(&corpus, "/home/reagan/development/search_engine_c/resources/HTMLFiles", count);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time spent: %f\n", time_spent);


    corpus_delete(&corpus, count);
    return(0);
}


int process_corpus(corpus* corpus, const char* filepath, u_int count)
{

    getDirectories(filepath, corpus->document_list, count);
    
    printf("#### starting tokenize %d files ####\n", count);
    for (u_int i = 0; i < count; ++i)
    {
        document *doc = &corpus->document_list[i];
        hashmap_create(&doc->word_frequency, DEFAULT_HASHMAP_SIZE);
        tokenize(corpus, doc);
        hashmap_printto_file(&doc->word_frequency, doc->outfile_name);
    }

    //printf("%lu\n", corpus.corpus_word_frequency.curr_size);
    //printf("%lu\n", corpus.document_frequency.curr_size);
   
    //remove single occurences
    hashmap new_corpus;
    hashmap_create(&new_corpus, DEFAULT_HASHMAP_SIZE);

    for (u_int i = 0; i < corpus->corpus_word_frequency.max_size; ++i)
    {
        if (corpus->corpus_word_frequency.table[i].frequency > 1)
        {
            hashmap_entry *entry;
            entry = hashmap_insert(&new_corpus, corpus->corpus_word_frequency.table[i].word);
            entry->frequency = corpus->corpus_word_frequency.table[i].frequency;
        }
    }
    hashmap_delete(&corpus->corpus_word_frequency);
    corpus->corpus_word_frequency = new_corpus;

    hashmap new_doc_freq;
    hashmap_create(&new_doc_freq, DEFAULT_HASHMAP_SIZE);

    for (u_int i = 0; i < corpus->document_frequency.max_size; ++i)
    {
        if (corpus->document_frequency.table[i].frequency > 1)
        {
            hashmap_entry *entry;
            entry = hashmap_insert(&new_doc_freq, corpus->document_frequency.table[i].word);
            entry->frequency = corpus->document_frequency.table[i].frequency;
        }
    }
    hashmap_delete(&corpus->document_frequency);
    corpus->document_frequency = new_doc_freq;

    u_int64_t post_size = 0;
    hashmap posting;
    hashmap_create(&posting, DEFAULT_HASHMAP_SIZE);

    for (u_int i = 0; i < count; ++i)
    {
        document *doc = &corpus->document_list[i];
        hashmap *map = &doc->word_frequency;
        u_int doc_len = map->curr_size;
        u_int sumofsquares = 0;

        for (u_int j = 0; j < map->max_size; ++j)
        {
            if (map->table[j].frequency != 0)
            {
                sumofsquares += pow(map->table[j].frequency, 2);
            }
        }
        float_t sqrtSoS = sqrt(sumofsquares);
        
        hashmap tfidf_list;
        hashmap_create(&tfidf_list, DEFAULT_HASHMAP_SIZE);

        for (u_int j = 0; j < map->max_size; ++j)
        {
            if (map->table[j].frequency != 0)
            {
                //printf("%s:%lu\n", tmp->table[j].word, tmp->table[j].frequency);
                hashmap_entry *secondary = hashmap_find(&corpus->corpus_word_frequency, map->table[j].word);
                if (secondary != NULL && secondary->frequency != 0)
                {
                    ++post_size;
                    float_t idf = log2(doc_len * secondary->frequency);
                    float_t tfidf = (float_t)(map->table[j].frequency * idf) / sqrtSoS; 
                    hashmap_entry *entry = hashmap_insert(&tfidf_list, map->table[j].word);
                    entry->frequency = (u_int)(tfidf * 1000000);
                    
                    char stfidf[64] = {0};
                    snprintf(stfidf, 64, "%d", (u_int)(tfidf * 1000000));
                    char post[MAX_WORD_LENGTH] = {0};
                    strcat(post, map->table[j].word);
                    strcat(post, ":");
                    strcat(post, doc->document_name); 
                    strcat(post, ":");
                    strcat(post, stfidf);
                    hashmap_insert(&posting, post);
                }
            }
        }
        

        char outfile[MAX_FILENAME_LEN] = { "../output/tfidf/" };
        strncat(outfile, corpus->document_list[i].document_name, MAX_FILENAME_LEN - 1);
        strcat(outfile, ".txt");
        hashmap_printto_file(&tfidf_list, outfile);
        hashmap_delete(&tfidf_list);
    }

    printf("Postings size: %lu\n", post_size);
    printf("Dictionary size: %lu\n", corpus->corpus_word_frequency.curr_size);

    typedef struct post_entry post_entry;
    struct post_entry
    {
        char *file_name[MAX_FILENAME_LEN];
        float_t weight;
    };
    typedef struct dict_entry dict_entry;
    struct dict_entry
    {
        char *word[MAX_WORD_LENGTH];
        u_int64_t start;
        u_int64_t length;
    };

    post_entry* postings;
    dict_entry* dictionary;

    postings = calloc(post_size, sizeof(post_entry));
    dictionary = calloc(corpus->corpus_word_frequency.curr_size, sizeof(dict_entry));

    hashmap_printto_file(&posting, "../output/post.txt");
    hashmap_printto_file(&corpus->corpus_word_frequency, "../output/all.txt");
    hashmap_printto_file(&corpus->document_frequency, "../output/freq.txt");

    return 0;
}
