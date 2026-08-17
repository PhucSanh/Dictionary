#include "dict_db.h"
#include "dict_types.h"
#include "third_party/sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict_kana.h"
const char* SCHEMA_SQL =        "CREATE TABLE words ("
                    "  id           INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "  word         TEXT NOT NULL,"
                    "  reading      TEXT NOT NULL,"
                    "  reading_hira TEXT NOT NULL,"
                    "  romaji       TEXT,"
                    "  part_of_speech          TEXT,"
                    "  meaning   TEXT,"
                    "  english   TEXT,"
                    "  level        TEXT"
                    ")";

struct DictDb {
    sqlite3 *handle;
    sqlite3_stmt *insert_stmt;
    char     last_error[256];
};
static void set_error(DictDb *db, const char *msg)
{
    if(db == NULL) return ;
    snprintf(db->last_error,sizeof db->last_error,"%s",msg);
}
static void copy_col(char*dist, size_t dst_size,sqlite3_stmt* stmt,int col){
  const unsigned char *v = sqlite3_column_text(stmt, col);
    snprintf(dist,dst_size,"%s",v?(const char*)v:"");
}
void dict_db_close(DictDb *db)
{
    if (db == NULL) return;
    sqlite3_finalize(db->insert_stmt);
    sqlite3_close(db->handle);
    free(db);
}
static int exec_sql(DictDb *db, const char *sql)
{
    if(db == NULL || sql == NULL) return DICT_ERR_ARG;
    char * err = NULL;
    if(sqlite3_exec(db->handle,sql,NULL,NULL,&err)!=SQLITE_OK){
        set_error(db, err);
        sqlite3_free(err);
        return DICT_ERR_DB;
    }
    return DICT_OK;
}
int	dict_db_create(const char *path,DictDb	**out){
    if(path == NULL || out == NULL) return DICT_ERR_ARG;
    remove(path);
    DictDb *db = (DictDb *)calloc(1, sizeof(DictDb));
    if (db == NULL) return DICT_ERR_NOMEM;
    if (sqlite3_open(path, &db->handle) != SQLITE_OK) {

        sqlite3_close(db->handle);
        free(db);
        return DICT_ERR_DB;
    }
    if(exec_sql(db,SCHEMA_SQL)!=DICT_OK){
        sqlite3_close(db->handle);
        free(db);
        return DICT_ERR_DB;

    }
    *out = db;
    return DICT_OK;
}
int dict_db_create_indexes(DictDb *db)
{
    return exec_sql(db,
                    "CREATE INDEX idx_words_word   ON words(word         COLLATE NOCASE);"
                    "CREATE INDEX idx_words_hira   ON words(reading_hira COLLATE NOCASE);"
                    "CREATE INDEX idx_words_romaji ON words(romaji       COLLATE NOCASE);"

          );
}
int dict_db_create_fts(DictDb *db)
{
    return exec_sql(db,
                    "CREATE VIRTUAL TABLE words_fts USING fts5(meaning, english);"
                    "INSERT INTO words_fts(rowid, meaning, english) "
                    "SELECT id, meaning, english FROM words;");
}
const char *dict_db_last_error(const DictDb *db)
{
    return db ? db->last_error : "";
}
int dict_db_begin(DictDb *db)    { return exec_sql(db, "BEGIN"); }
int dict_db_commit(DictDb *db)   { return exec_sql(db, "COMMIT"); }
int dict_db_rollback(DictDb *db) { return exec_sql(db, "ROLLBACK"); }



int dict_db_insert(DictDb *db, const DictEntry *e)
{
    if (db == NULL || e == NULL) return DICT_ERR_ARG;

    if (db->insert_stmt == NULL) {
        const char *sql =
            "INSERT INTO words "
            "(word, reading, reading_hira, romaji, part_of_speech, meaning, english, level) "
            "VALUES (?,?,?,?,?,?,?,?)";
        if (sqlite3_prepare_v2(db->handle, sql, -1, &db->insert_stmt, NULL) != SQLITE_OK) {
            set_error(db, sqlite3_errmsg(db->handle));
            return DICT_ERR_DB;
        }
    }

    sqlite3_bind_text(db->insert_stmt, 1, e->word,           -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(db->insert_stmt, 2, e->reading,        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(db->insert_stmt, 3, e->reading_hira,   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(db->insert_stmt, 4, e->romaji,         -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(db->insert_stmt, 5, e->part_of_speech, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(db->insert_stmt, 6, e->meaning,        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(db->insert_stmt, 7, e->english,        -1, SQLITE_TRANSIENT);

    if (e->level[0] == '\0')
        sqlite3_bind_null(db->insert_stmt, 8);
    else
        sqlite3_bind_text(db->insert_stmt, 8, e->level, -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(db->insert_stmt);
    sqlite3_reset(db->insert_stmt);

    if (rc != SQLITE_DONE) {
        set_error(db, sqlite3_errmsg(db->handle));
        return DICT_ERR_DB;
    }
    return DICT_OK;
}

int dict_db_open(const char *path, DictDb **out)
{
    if(path == NULL || out == NULL) return DICT_ERR_ARG;
    DictDb* db = (DictDb *)calloc(1,sizeof(DictDb));
    if (db == NULL) return DICT_ERR_NOMEM;
    if(sqlite3_open_v2(path,&db->handle,SQLITE_OPEN_READONLY,NULL)!=SQLITE_OK){
        sqlite3_close(db->handle);
        free(db);
        return DICT_ERR_DB;
    }
    *out = db;
    return DICT_OK;

}

int dict_db_search(DictDb *db, const char *search, int limit,int offset, DictEntryList *out)
{
   if (db == NULL || search == NULL || out== NULL) return DICT_ERR_ARG;
   char hira[DICT_READING_LEN];
   dict_kana_to_hiragana(search,hira,sizeof hira);
   char pat_raw[DICT_READING_LEN + 4];
   char pat_hira[DICT_READING_LEN + 4];
   snprintf(pat_raw,  sizeof pat_raw,  "%s%%", search);
   snprintf(pat_hira, sizeof pat_hira, "%s%%", hira);
   sqlite3_stmt *stmt = NULL;
   const char *sql =
       "SELECT id, word, reading, reading_hira, romaji, "
       "       part_of_speech, meaning, english, level "
       "FROM words "
       "WHERE word LIKE ?1 OR reading_hira LIKE ?2 OR romaji LIKE ?1 "
       "ORDER BY "
       "  CASE WHEN word = ?3 OR reading_hira = ?4 OR romaji = ?3 "
       "       THEN 0 ELSE 1 END, "
       "  length(word), "
       "  word "
       "LIMIT ?5 OFFSET ?6";


   if (sqlite3_prepare_v2(db->handle, sql, -1, &stmt, NULL) != SQLITE_OK) {
       set_error(db, sqlite3_errmsg(db->handle));
       return DICT_ERR_DB;
   }
   sqlite3_bind_text(stmt, 1, pat_raw,  -1, SQLITE_TRANSIENT);
   sqlite3_bind_text(stmt, 2, pat_hira, -1, SQLITE_TRANSIENT);
   sqlite3_bind_text(stmt, 3, search,        -1, SQLITE_TRANSIENT);
   sqlite3_bind_text(stmt, 4, hira,     -1, SQLITE_TRANSIENT);
   sqlite3_bind_int (stmt, 5, limit);
   sqlite3_bind_int (stmt, 6, offset);
   if (dict_entry_list_init(out, 16) != DICT_OK) {
       sqlite3_finalize(stmt);
       return DICT_ERR_NOMEM;
   }
   while (sqlite3_step(stmt) == SQLITE_ROW) {
       DictEntry e;
       memset(&e, 0, sizeof e);

       e.id = sqlite3_column_int(stmt, 0);
       copy_col(e.word,           sizeof e.word,           stmt, 1);
       copy_col(e.reading,        sizeof e.reading,        stmt, 2);
       copy_col(e.reading_hira,   sizeof e.reading_hira,   stmt, 3);
       copy_col(e.romaji,         sizeof e.romaji,         stmt, 4);
       copy_col(e.part_of_speech, sizeof e.part_of_speech, stmt, 5);
       copy_col(e.meaning,        sizeof e.meaning,        stmt, 6);
       copy_col(e.english,        sizeof e.english,        stmt, 7);
       copy_col(e.level,          sizeof e.level,          stmt, 8);

       dict_entry_list_push(out, &e);
   }
   sqlite3_finalize(stmt);
   return DICT_OK;

}

int dict_db_search_meaning(DictDb *db, const char *search, int limit,int offset, DictEntryList *out)
{
    if (db == NULL || search == NULL || out== NULL) return DICT_ERR_ARG;

     sqlite3_stmt *stmt = NULL;
    const char *sql =
        "SELECT w.id, w.word, w.reading, w.reading_hira, w.romaji, "
        "       w.part_of_speech, w.meaning, w.english, w.level "
        "FROM words_fts f "
        "JOIN words w ON w.id = f.rowid "
        "WHERE f.words_fts MATCH ?1 "
        "ORDER BY "
        "  CASE WHEN w.meaning LIKE ?2 OR w.english LIKE ?2 THEN 0 ELSE 1 END, "
        "  bm25(words_fts) "
        "LIMIT ?3 OFFSET ?4";
     char pat[DICT_MEANING_LEN + 4];
     snprintf(pat, sizeof pat, "%%%s%%", search);
     if (sqlite3_prepare_v2(db->handle, sql, -1, &stmt, NULL) != SQLITE_OK) {
         set_error(db, sqlite3_errmsg(db->handle));
         return DICT_ERR_DB;
     }
     sqlite3_bind_text(stmt, 1, search, -1, SQLITE_TRANSIENT);
     sqlite3_bind_text(stmt, 2, pat,    -1, SQLITE_TRANSIENT);
     sqlite3_bind_int (stmt, 3, limit);
     sqlite3_bind_int (stmt, 4, offset);
     if (dict_entry_list_init(out, 16) != DICT_OK) {
         sqlite3_finalize(stmt);
         return DICT_ERR_NOMEM;
     }
     while (sqlite3_step(stmt) == SQLITE_ROW) {
         DictEntry e;
         memset(&e, 0, sizeof e);

         e.id = sqlite3_column_int(stmt, 0);
         copy_col(e.word,           sizeof e.word,           stmt, 1);
         copy_col(e.reading,        sizeof e.reading,        stmt, 2);
         copy_col(e.reading_hira,   sizeof e.reading_hira,   stmt, 3);
         copy_col(e.romaji,         sizeof e.romaji,         stmt, 4);
         copy_col(e.part_of_speech, sizeof e.part_of_speech, stmt, 5);
         copy_col(e.meaning,        sizeof e.meaning,        stmt, 6);
         copy_col(e.english,        sizeof e.english,        stmt, 7);
         copy_col(e.level,          sizeof e.level,          stmt, 8);

         dict_entry_list_push(out, &e);
     }
     sqlite3_finalize(stmt);
     return DICT_OK;


}

int dict_db_count_total(DictDb *db, const char *q,int *out_total)
{
    if(db == NULL || q == NULL) return DICT_ERR_ARG;
    const char *sql = NULL;
    const int is_meaning = (dict_kana_detect(q) == DICT_INPUT_VIETNAMESE);
    if(is_meaning){
        sql = "SELECT COUNT(*) FROM words_fts WHERE words_fts MATCH ?1";
    }
    else {
        sql = "SELECT COUNT(*) FROM words "
              "WHERE word LIKE ?1 OR reading_hira LIKE ?2 OR romaji LIKE ?1";
    }
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(db->handle, sql, -1, &stmt, NULL) != SQLITE_OK) {
        set_error(db, sqlite3_errmsg(db->handle));
        return DICT_ERR_DB;
    }
    char hira[DICT_READING_LEN];
    char pat_raw[DICT_READING_LEN + 4];
    char pat_hira[DICT_READING_LEN + 4];

    if (is_meaning) {
        sqlite3_bind_text(stmt, 1, q, -1, SQLITE_TRANSIENT);
    } else {
        dict_kana_to_hiragana(q, hira, sizeof hira);
        snprintf(pat_raw,  sizeof pat_raw,  "%s%%", q);
        snprintf(pat_hira, sizeof pat_hira, "%s%%", hira);
        sqlite3_bind_text(stmt, 1, pat_raw,  -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, pat_hira, -1, SQLITE_TRANSIENT);
    }
    int total = 0;
    if(sqlite3_step(stmt) == SQLITE_ROW){
        total = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    *out_total = total;
    return DICT_OK;

}
