#ifndef OBJECTS_H
#define OBJECTS_H

#include <string.h>
#include <sys/types.h>

#include "hashmap/hashmap.h"

#define MAX_FILENAME_LEN 256
#define DEFAULT_HASHMAP_SIZE 2048

typedef struct document document;
struct document
{
    hashmap word_frequency;
    char infile_name[MAX_FILENAME_LEN];
    char outfile_name[MAX_FILENAME_LEN];
    char document_name[MAX_FILENAME_LEN];
};

typedef struct corpus corpus;
struct corpus
{
    hashmap corpus_word_frequency;
    hashmap document_frequency;
    hashmap stoplist;
    document *document_list;
};

void corpus_init(corpus* corpus, u_int doc_count);
void corpus_delete(corpus* corpus, u_int doc_count);
int tokenize(corpus* corpus, document* document);
int process_corpus(corpus* corpus, const char* filepath, u_int count);
void getDirectories(const char* dir_name, document* document, u_int doc_count);

#endif /* OBJECTS_H */ 
