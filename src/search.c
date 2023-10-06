#include <time.h>

#include "hashtable/hashtable.h"
#include "arraylist/arraylist.h"
#include "corpus.h"
#include "query.h"

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

int main(int argc, char *argv[])
{
    char directory[MAX_FILENAME_LEN] = { "/home/reagan/development/search_engine_c/resources/HTMLFiles" };
    char* arr[] = {"godot", "unity", "industry", "shareholders"};
    char* arr2[] = {"accrue", "godot", "computer"};

    if (argc < 2) 
    {
        search(arr2, 3);
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

