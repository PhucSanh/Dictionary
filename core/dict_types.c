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

int dict_note_list_init(DictNoteList *list, int capacity)
{
    if(list == NULL || capacity<=0) return DICT_ERR_ARG;
    DictNote *p = (DictNote *) malloc(capacity*sizeof(DictNote));
    if(p == NULL) return DICT_ERR_NOMEM;
    list->items = p;
    list->count = 0;
    list->capacity = capacity;
    return DICT_OK;

}

int dict_note_list_push(DictNoteList *list, const DictNote *n)
{
    if(list == NULL || n==NULL) return DICT_ERR_ARG;
    if(list->capacity == list->count){
        DictNote * p = realloc(list->items,list->capacity*2*sizeof(DictNote));
        if(p == NULL) return DICT_ERR_NOMEM;
        list->items = p;
        list->capacity = list->capacity*2;
    }
    list->items[list->count] = *n;
    list->count++;
    return DICT_OK;

}

void dict_note_list_free(DictNoteList *list)
{
    if(list == NULL) return ;
    free(list->items);
    list->items=NULL;
    list->count = 0;
    list->capacity = 0;


}

int dict_category_list_init(DictCategoryList *list, int capacity)
{
    if(list == NULL || capacity<=0) return DICT_ERR_ARG;
    DictCategory *p = (DictCategory *) malloc(capacity*sizeof(DictCategory));
    if(p == NULL) return DICT_ERR_NOMEM;
    list->items = p;
    list->count = 0;
    list->capacity = capacity;
    return DICT_OK;

}

int dict_category_list_push(DictCategoryList *list, const DictCategory *c)
{
    if(list == NULL || c == NULL) return DICT_ERR_ARG;
    if(list->capacity == list->count){
        DictCategory *p = realloc(list->items,list->capacity*2*sizeof(DictCategory));
        if(p == NULL) return DICT_ERR_NOMEM;
        list->items = p;
        list->capacity = list->capacity*2;
    }
    list->items[list->count] = *c;
    list->count++;
    return DICT_OK;

}

void dict_category_list_free(DictCategoryList *list)
{
    if(list == NULL) return ;
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;

}
