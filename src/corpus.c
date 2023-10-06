#include <ctype.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arraylist/arraylist.h"
#include "hashmap/hashmap.h"
#include "postings/postings.h"
#include "dictionary/dictionary.h"
#include "corpus.h"


void addToken(char* token, FILE* outfile, hashmap* file_hm, corpus* corpus)
{
    if (strlen(token) > 2)
    {
        if (!hashmap_exists(file_hm, token))
        {
            hashmap_insert(&corpus->document_frequency, token);
        }
        hashmap_insert(file_hm, token);
        hashmap_insert(&corpus->corpus_word_frequency, token);
    }
    token[0] = '\0';
}

int checkToken(char* token)
{
    if(strcmp(token, "\t") == 0 || strcmp(token, "\n") == 0 ||
        strcmp(token, " ") == 0 || token[0] == '\0')
    {
        return 0;
    }
    return 1;
}

int tokenize(corpus* corpus, document* document)
{
    FILE *infile = fopen(document->infile_name, "r");
    if (infile == NULL || infile == 0) {return -1;}
    
    FILE *outfile = fopen(document->outfile_name, "w+");
    if (outfile == NULL || outfile == 0) {fclose(infile); return -1;}

    char token[1028] = {0};
    char curr = fgetc(infile);

    while (curr != EOF)
    {
        curr = tolower(curr);
        if (curr == ' ' || curr == '\n' || curr == '\t')
        {
            if (checkToken(token))
            {
                if (hashmap_exists(&corpus->stoplist, token) == 0)
                {
                    addToken(token, outfile, &document->word_frequency, corpus);
                }
                else {token[0] = '\0';}
            }
            curr = fgetc(infile);
        }
        else if (curr == '<')
        {
            if (checkToken(token))
            {
                if (hashmap_exists(&corpus->stoplist, token) == 0)
                {
                    addToken(token, outfile, &document->word_frequency, corpus);
                }
                else {token[0] = '\0';}
            }

            while (curr != '>')
            {
                if (curr == EOF)
                {
                    break;
                }
                curr = fgetc(infile);
            }
        }
        else
        {
            if (isalpha(curr))
            {
                strncat(token, &curr, 1);
            }
            curr = fgetc(infile);
        }
    }
    //hashmap_printto_file(&document->word_frequency, document->outfile_name);

    fclose(infile); 
    fclose(outfile);
    return 0;
}

void getDirectories(const char* dir_name, document* document, u_int doc_count)
{
    DIR *inputFolder = opendir(dir_name);
    struct dirent *dir;

    u_int count = 0;

    if (inputFolder)
    {
        while (((dir = readdir(inputFolder)) != NULL) && count <= doc_count - 1)
        {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
            {
                snprintf(document[count].infile_name, MAX_FILENAME_LEN, "%s%s%s", dir_name, "/", dir->d_name);
                snprintf(document[count].outfile_name, MAX_FILENAME_LEN, "%s%s%s", "../output/freq/", dir->d_name, ".txt");
                // TODO create freq directory if not exists

                snprintf(document[count].document_name, MAX_FILENAME_LEN, "%s", dir->d_name);
                count ++;
            }
        }
        closedir(inputFolder);
    }
}

void get_stoplist(hashmap *stoplist, const char* file_path)
{
    FILE* stoplist_file = fopen(file_path, "r");
    char stopword[MAX_WORD_LENGTH] = { 0 };
    hashmap_create(stoplist, 2048);
    while (fgets(stopword, MAX_WORD_LENGTH, stoplist_file) != NULL)
    {
        stopword[strlen(stopword)-1] = '\0';
        hashmap_insert(stoplist, stopword);
    }
    fclose(stoplist_file);
}

void corpus_init(corpus* corpus, u_int doc_count)
{
    hashmap_create(&corpus->document_frequency, DEFAULT_HASHMAP_SIZE);
    hashmap_create(&corpus->corpus_word_frequency, DEFAULT_HASHMAP_SIZE);
    hashmap_create(&corpus->stoplist, DEFAULT_HASHMAP_SIZE);

    get_stoplist(&corpus->stoplist, "../resources/stoplist.txt");

    corpus->document_list = calloc(doc_count, sizeof(document));
    for (u_int i = 0; i < doc_count; ++i)
    {
        hashmap_create(&corpus->document_list[i].word_frequency, DEFAULT_HASHMAP_SIZE);
        corpus->document_list[i].infile_name[0] = '\0';
        corpus->document_list[i].outfile_name[0] = '\0';
    }
}

void corpus_delete(corpus* corpus, u_int doc_count)
{
    hashmap_delete(&corpus->document_frequency);
    hashmap_delete(&corpus->corpus_word_frequency);
    hashmap_delete(&corpus->stoplist);

    for (u_int i = 0; i < doc_count; ++i)
    {
        hashmap_delete(&corpus->document_list[i].word_frequency);
    }
}

int calc_tf_idf()
{
   return 0; 
}


typedef struct dict_t dict_t;
struct dict_t {
    char word[MAX_WORD_LENGTH];
    size_t length;
    size_t index_start;
};

typedef struct post_v_t post_v_t;
struct post_v_t {
    char file_name[MAX_FILE_LENGTH];
    char word[MAX_WORD_LENGTH];
    float_t weight;
};

int dict_t_cmp(const void* l, const void* r)
{
    dict_entry* first = (dict_entry*)l;
    dict_entry* second = (dict_entry*)r;
    return strcmp(first->word, second->word);
}

void dict_t_load(const void *d, char *str)
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

void dict_t_stream(const void *d, FILE* stream)
{
    dict_t* data = (dict_t*)d;
    fprintf(stream, "%s:%zu:%lu\n", data->word, data->length, data->index_start);
}

int post_v_t_cmp(const void* l, const void* r)
{
    post_entry* first = (post_entry*)l;
    post_entry* second = (post_entry*)r;

    int ret = strcmp(first->word, second->word);

    if (ret == 0) {
        return (int) ((second->weight - first->weight)*1000000); 
    } else return ret;
}

void post_v_t_load(const void *d, char *str)
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

void post_v_t_stream(const void *d, FILE* stream)
{
    post_entry* data = (post_entry*)d;
    fprintf(stream, "%s:%s:%f\n", data->word, data->file_name, data->weight);
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

    printf("BEFORE - corpus:%lu - document:%lu\n", corpus->corpus_word_frequency.curr_size, corpus->document_frequency.curr_size);
   
    //remove single occurences in entire corpus
    hashmap new_word_freq;
    hashmap_create(&new_word_freq, DEFAULT_HASHMAP_SIZE);
    hashmap new_doc_freq;
    hashmap_create(&new_doc_freq, DEFAULT_HASHMAP_SIZE);
    for (u_int i = 0; i < corpus->corpus_word_frequency.max_size; ++i)
    {
        if (corpus->corpus_word_frequency.table[i].frequency > 1 && corpus->document_frequency.table[i].frequency > 1)
        {
            hashmap_entry *entry;
            entry = hashmap_insert(&new_word_freq, corpus->corpus_word_frequency.table[i].word);
            entry->frequency = corpus->corpus_word_frequency.table[i].frequency;

            hashmap_entry *doc_entry;
            doc_entry = hashmap_insert(&new_doc_freq, corpus->document_frequency.table[i].word);
            doc_entry->frequency = corpus->document_frequency.table[i].frequency;
        }
    }
    hashmap_delete(&corpus->corpus_word_frequency);
    corpus->corpus_word_frequency = new_word_freq;
    hashmap_delete(&corpus->document_frequency);
    corpus->document_frequency = new_doc_freq;

    printf("AFTER - corpus:%lu - document:%lu\n", corpus->corpus_word_frequency.curr_size, corpus->document_frequency.curr_size);

    u_int64_t post_size = 0;
    postings posting;
    postings_init(&posting);
    arraylist *post1 = list_init(sizeof(post_v_t), post_v_t_cmp, post_v_t_stream);

    hashmap new;
    hashmap_create(&new, DEFAULT_HASHMAP_SIZE);

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
                    
                    postings_add(&posting, map->table[j].word, doc->document_name, tfidf);
                    //list_append(post, &(post_v_t){.file_name=doc->document_name, .word=map->table[j].word, .weight=tfidf});
                    hashmap_insert(&new, map->table[j].word);
                }
            }
        }

        char outfile[MAX_FILENAME_LEN];
        snprintf(outfile, MAX_FILENAME_LEN, "%s%s%s", "../output/tfidf/", corpus->document_list[i].document_name, ".txt");
        //hashmap_printto_file(&tfidf_list, outfile);

        hashmap_delete(&tfidf_list);
    }

    postings_sort(&posting);

    dictionary dict;
    dictionary_init(&dict, &posting, new.curr_size);

    printf("Postings size: %lu\n", post_size);
    printf("Dictionary size: %lu\n", new.curr_size);

    dictionary_printto_file(&dict, "../output/dict.txt");
    postings_printto_file(&posting, "../output/post.txt");
    hashmap_printto_file(&corpus->corpus_word_frequency, "../output/all.txt");
    hashmap_printto_file(&corpus->document_frequency, "../output/freq.txt");

    return 0;
}
