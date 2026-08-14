#ifndef DICT_TYPES_H
#define DICT_TYPES_H
#include <stddef.h>
    #ifdef __cplusplus
    extern "C" {
    #endif
        #define DICT_WORD_LEN     64
        #define DICT_READING_LEN  64
        #define DICT_ROMAJI_LEN   32
        #define DICT_POS_LEN      32
        #define DICT_MEANING_LEN  128
        #define DICT_ENGLISH_LEN  192
        #define DICT_LEVEL_LEN     8

    typedef enum {
        DICT_OK        =  0,
        DICT_ERR_ARG   = -1,
        DICT_ERR_IO    = -2,
        DICT_ERR_NOMEM = -3,
        DICT_ERR_DB    = -4
    } DictStatus;
    typedef struct {
        int  id;
        char word[DICT_WORD_LEN];
        char reading[DICT_READING_LEN];
        char reading_hira[DICT_READING_LEN];
        char romaji[DICT_ROMAJI_LEN];
        char part_of_speech[DICT_POS_LEN];
        char meaning[DICT_MEANING_LEN];
        char english[DICT_ENGLISH_LEN];
        char level[DICT_LEVEL_LEN];
    } DictEntry;
    typedef struct{
        DictEntry *items;
        int count;
        int capacity;

    }DictEntryList;
    int dict_entry_list_init(DictEntryList *list, int capacity);


    int dict_entry_list_push(DictEntryList *list, const DictEntry *e);

    void dict_entry_list_free(DictEntryList *list);
    #ifdef __cplusplus
    }
    #endif

#endif