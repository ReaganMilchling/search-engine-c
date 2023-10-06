#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

typedef int (*cb_cmp)(const void *l, const void *r);
typedef void (*cb_stream)(const void *d, FILE* stream);
typedef void (*cb_load)(const void *d, char *s);

typedef struct arraylist arraylist;

arraylist* list_init(size_t size, cb_cmp fn_cmp, cb_stream fn_stream);
void list_load(arraylist* list, const char* filepath, cb_load fn_load, bool sorted);
void list_transfer(arraylist* list, void *transfer, size_t length, size_t count);
void list_destroy(arraylist* list);

void list_append(arraylist* list, void *value);
void list_insert(arraylist* list, void *value, size_t index);
//void list_remove(arraylist* list, size_t index);
void list_sort(arraylist* list);

void *list_get(arraylist* list, size_t index);
void *list_find(arraylist* list, void *value);

void list_print_file(arraylist* list, const char* filepath);
void list_print_console(arraylist* list, size_t index);

#endif /* ARRAYLIST_H */
