#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "hashmap/hashmap.h"
#include "objects.h"


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
    if(strcmp(token, "\t") == 0 || strcmp(token, "\n") == 0 || strcmp(token, " ") == 0)
    {
        token[0] = '\0';
        return 0;
    }
    if (token[0] == '\0') return 0;
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
            while (curr != '>')
            {
                if (curr == EOF)
                {
                    break;
                }
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
        }
        else
        {
            if (!isdigit(curr) && !ispunct(curr) && curr != '\n' && curr != '\t')
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
    DIR *inputFolder;
    struct dirent *dir;

    u_int count = 0;
    const char* dirName = dir_name;

    inputFolder = opendir(dirName);
    if (inputFolder)
    {
        while (((dir = readdir(inputFolder)) != NULL) && count <= doc_count- 1)
        {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
            {
                char infile[MAX_FILENAME_LEN] = { 0 };
                strcat(infile, dirName); 
                strcat(infile, "/"); 
                strcat(infile, dir->d_name); 
                
                char outfile[MAX_FILENAME_LEN] = { "../output/freq/" };
                //create directory if not exists
                strncat(outfile, dir->d_name, strlen(dir->d_name)-5);
                strcat(outfile, ".txt");
                //printf("%s\n", outfile);

                char name[MAX_FILENAME_LEN] = { 0 };
                strncat(name, dir->d_name, strlen(dir->d_name)-5);

                memcpy(document[count].document_name, name, MAX_FILENAME_LEN);
                memcpy(document[count].infile_name, infile, MAX_FILENAME_LEN);
                memcpy(document[count].outfile_name, outfile, MAX_FILENAME_LEN);
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
