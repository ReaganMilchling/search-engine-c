#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

uint64_t hashstring(const char *str)
{
    //http://www.cse.yorku.ca/~oz/hash.html
    //djb2
    uint64_t hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void hashmap_create(hashmap *map, uint64_t size)
{
    if (size < 512) {size = 512;}

    memset(map, 0, sizeof(*map));
    map->curr_size = 0;
    map->max_size = size;
    map->table = calloc(size, sizeof(hashmap_entry));

    for (uint64_t i = 0; i < map->max_size - 1; ++i)
    {
        map->table[i].word[0] = '\0';
        map->table[i].frequency = 0;
    }
}

void hashmap_delete(hashmap *map)
{
    free(map->table);
    memset(map, 0, sizeof(*map));
}

bool hashmap_exists(hashmap *map, const char* key)
{
    uint64_t hash = hashstring(key);
    uint64_t index = hash % map->max_size;

    while (map->table[index].frequency != 0)
    {
        if (strcmp(map->table[index].word, key) == 0)
        {
            return 1;
        }
        ++index;        
        if (index >= map->max_size) {index = 0;}
    }
    return 0;
}

hashmap_entry* hashmap_find(hashmap *map, const char* key)
{
    uint64_t hash = hashstring(key);
    uint64_t index = hash % map->max_size;

    while (map->table[index].frequency != 0)
    {
        //printf("1:%s 2:%s\n", map->table[index].word, key);
        if (strncmp(map->table[index].word, key, MAX_WORD_LENGTH) == 0)
        {
            //printf("%s\n", key);
            return &map->table[index];
        }
        ++index;        
        if (index >= map->max_size - 1) {index = 0;}
    }
    return NULL;
}

hashmap_entry* hashmap_insert(hashmap *map, const char* key)
{
    if (strlen(key) > 127) {return (void*)0;}

    if (map->curr_size * 2 >= map->max_size)
    {
        uint64_t newsize = map->max_size * 2;
        hashmap resizedmap;
        hashmap_create(&resizedmap, newsize);

        for (uint64_t i = 0; i < map->max_size - 1; ++i)
        {
            if (map->table[i].frequency != 0)
            {
                hashmap_entry* temp = hashmap_insert(&resizedmap, map->table[i].word);
                temp->frequency = map->table[i].frequency;
            }
        }
        hashmap_delete(map);
        map->max_size = newsize;
        map->curr_size = resizedmap.curr_size;
        map->table = resizedmap.table;
    }

    uint64_t hash = hashstring(key);
    uint64_t index = hash % map->max_size;
    
    while (map->table[index].frequency != 0)
    {
        if (strcmp(map->table[index].word, key) == 0)
        {
            ++map->table[index].frequency;
            return &map->table[index];
        }
        ++index;
        if (index >= map->max_size) {index = 0;}
    }

    strcpy(map->table[index].word, key);
    ++map->table[index].frequency;
    ++map->curr_size;

    return &map->table[index];
}

void hashmap_print(hashmap *map)
{
    for (uint64_t i = 0; i < map->max_size; ++i)
    {
        if (map->table[i].frequency != 0)
        {
            printf("%s:%lu\n", map->table[i].word, map->table[i].frequency);
        }
    }
}

void hashmap_printto_file(hashmap *map, const char* filepath)
{
    FILE* outfile = fopen(filepath, "w+");
    if (outfile == NULL || outfile == 0) {return;}
    
    //printf("%s\n", filepath);
    for (uint64_t i = 0; i < map->max_size; ++i)
    {
        if (map->table[i].frequency != 0)
        {
            fprintf(outfile, "%s:%lu\n", map->table[i].word, map->table[i].frequency);
        }
    }
    fclose(outfile);
}

