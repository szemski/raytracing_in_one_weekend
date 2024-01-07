#include "hittable.h"
#include "malloc.h"
#include "process.h"

#define MIN_ARRAY_LIST_SIZE 10

void hittable_array_list_init(hittable_array_list* list)
{
    hittable* data = malloc(sizeof(hittable) * MIN_ARRAY_LIST_SIZE);

    if (!data) exit(1);

    list->size = 0;
    list->capacity = MIN_ARRAY_LIST_SIZE;
    list->data = data;
}

void hittable_array_list_delete(hittable_array_list* list)
{
    free(list->data);
    list->size = 0;
    list->capacity = 0;
    list->data = NULL;
}

void hittable_array_list_add(hittable_array_list* list, hittable item)
{
    if (list->size >= list->capacity)
    {
        hittable* data = realloc(list->data, list->capacity * 2 * sizeof(hittable));
        if (!data) exit(1);
        list->data = data;
        list->capacity *= 2;
    }

    list->data[list->size++] = item;
}

#undef MIN_ARRAY_LIST_SIZE
