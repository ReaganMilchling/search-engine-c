#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "hashmap/hashmap.h"

int main(int argc, char *argv[])
{
    FILE* infile = fopen("text/stoplist.txt", "r");
    char stopword[64] = { 0 };
    
    hashmap stoplist;
    hashmap_create(&stoplist, 128);
    
    hashmap_insert(&stoplist, "word");
    hashmap_insert(&stoplist, "test");
    hashmap_insert(&stoplist, "computer");
    hashmap_insert(&stoplist, "keyboard");
    hashmap_insert(&stoplist, "mouse");
    hashmap_insert(&stoplist, "word");
    
    char* word = "word";
    char* dne = "reagan";

    hashmap_entry* he = hashmap_find(&stoplist, word);
    printf("%s\n", he->word);
    
    he = hashmap_find(&stoplist, dne);
    if (he != NULL)
    {
        printf("%s\n", he->word);
    }
    //hashmap_print(&stoplist);
    
    hashmap_delete(&stoplist);

    printf("\n");
    return(0);
}


