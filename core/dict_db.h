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
int dict_db_count(DictDb *db, const char *q, int is_meaning, int *out_total);
int	dict_db_attach_user(DictDb	*db,const  char	*user_db_path);
int	dict_db_add_history(DictDb	*db,int	entry_id);
int	dict_db_list_history(DictDb	*db,int	limit,int offset,DictEntryList	*out);
int	dict_db_count_history(DictDb *db, int *out_total);
int	dict_db_clear_history(DictDb*db);
int	dict_db_toggle_favorite(DictDb*db,int	entry_id,int *out_is_favorite);
int	dict_db_is_favorite(DictDb*db,int	entry_id,	int	*out);
int	dict_db_list_favorites(DictDb *db,	int	limit,	DictEntryList *out);
int dict_db_add_note(DictDb *db, const DictNote *n, int *out_note_id);

int dict_db_update_note(DictDb *db, const DictNote *n);

int dict_db_list_notes(DictDb *db, int entry_id, DictNoteList *out);

int dict_db_list_favorite_notes(DictDb *db, int limit, DictNoteList *out);

int dict_db_delete_note(DictDb *db, int note_id);

int dict_db_list_categories(DictDb *db, DictCategoryList *out);

int dict_db_add_category(DictDb *db, const char *name, int *out_id);

int dict_db_rename_category(DictDb *db, int category_id, const char *name);

int dict_db_delete_category(DictDb *db, int category_id);

int dict_db_add_favorite(DictDb *db, int entry_id, const int *category_ids, int n);

int dict_db_remove_favorite(DictDb *db, int entry_id);

int dict_db_set_favorite_categories(DictDb *db, int entry_id, const int *category_ids, int n);

int dict_db_list_entry_categories(DictDb *db, int entry_id, DictCategoryList *out);

int dict_db_list_favorites_in_category(DictDb *db, int category_id, int limit, int offset, DictEntryList *out);

int dict_db_count_favorites(DictDb *db, int category_id, int *out_total);

int dict_db_list_favorite_notes_in_category(DictDb *db, int category_id, int limit, DictNoteList *out);
#endif // DICT_DB_H
