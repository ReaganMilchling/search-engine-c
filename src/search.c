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


int main(int argc, char *argv[])
{
    printf("Number of args %d\n", argc);

    char directory[MAX_FILENAME_LEN] = { "/home/reagan/development/search_engine_c/resources/HTMLFiles" };

    if (argc == 2) {
        memcpy(directory, argv[1], MAX_FILENAME_LEN);
    }

    printf("File directory: %s\n", directory);

    clock_t begin = clock();

    u_int count = 504; 
    corpus corpus;
    corpus_init(&corpus, count);
    process_corpus(&corpus, directory, count);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time spent: %f\n", time_spent);


    corpus_delete(&corpus, count);
    return(0);
}


