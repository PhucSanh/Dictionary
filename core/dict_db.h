#ifndef DICT_DB_H
#define DICT_DB_H

#include "dict_types.h"

typedef struct DictDb DictDb;
int  dict_db_create(const char *path, DictDb **out);
void dict_db_close(DictDb *db);
const char *dict_db_last_error(const DictDb *db);
int  dict_db_begin(DictDb *db);
int  dict_db_commit(DictDb *db);
int  dict_db_rollback(DictDb *db);
int  dict_db_insert(DictDb *db, const DictEntry *e);
int  dict_db_create_indexes(DictDb *db);
int  dict_db_create_fts(DictDb *db);
int  dict_db_create(const char *path, DictDb **out);
int  dict_db_open(const char *path, DictDb **out);
void dict_db_close(DictDb *db);
int dict_db_search(DictDb *db, const char *q, int limit, int offset, DictEntryList *out);
int dict_db_search_meaning(DictDb *db, const char *q, int limit, int offset, DictEntryList *out);
int dict_db_count_total(DictDb *db,const char*q,int * out_total);
#endif // DICT_DB_H
