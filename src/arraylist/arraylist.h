#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define INITIAL_LIST_SIZE 1024
#define GROWTH_FACTOR 2

typedef int (*callback_cmp)(const void *l, const void *r);
typedef void (*callback_stream)(const void *d, FILE* stream);
typedef void (*callback_load)(const void *d, char *s);

typedef struct ArrayList ArrayList;
struct ArrayList {
    void *list;
    size_t size;

    bool _sorted;
    size_t _max_size;
    size_t _type_byte_size;
    callback_cmp _fn_cmp;
    callback_stream _fn_stream;
};

void list_init(ArrayList* arraylist, size_t size, callback_cmp fn_cmp, callback_stream fn_stream);
void list_load(ArrayList* arraylist, const char* filepath, callback_load fn_load);
void list_destroy(ArrayList* arraylist);

void list_append(ArrayList* arraylist, void *value);
void list_insert(ArrayList* arraylist, void *value, size_t index);
void list_sort(ArrayList* arraylist);

void *list_get(ArrayList* arraylist, size_t index);
void *list_find(ArrayList* arraylist, void *value);

void list_print_file(ArrayList* arraylist, const char* filepath);
void list_print_console(ArrayList* arraylist, size_t index);

#endif /* ARRAYLIST_H */
