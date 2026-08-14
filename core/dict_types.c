#include "dict_types.h"
#include <stdlib.h>

int dict_entry_list_init(DictEntryList *list, int capacity)
{
    if(list == NULL || capacity<=0) return DICT_ERR_ARG;
    DictEntry *p = (DictEntry *)malloc(capacity * sizeof(DictEntry));
    if (p == NULL) return DICT_ERR_NOMEM;
    list->items = p;
    list->capacity = capacity;
    list->count = 0;
    return DICT_OK;

}


int dict_entry_list_push(DictEntryList *list, const DictEntry *e)
{
    if(list == NULL || e == NULL) return DICT_ERR_ARG;
    if(list->count == list->capacity){
        DictEntry *p = realloc(list->items,list->capacity*2*sizeof(DictEntry));
        if (p == NULL) return DICT_ERR_NOMEM;
        list->items = p;
        list->capacity = list->capacity*2;
    }
    list->items[list->count] = *e;
    list->count++;
    return DICT_OK;

}

void dict_entry_list_free(DictEntryList *list)
{
    if(list == NULL) return ;
    free(list->items);
    list->items=NULL;
    list->count = 0;
    list->capacity = 0;

}
