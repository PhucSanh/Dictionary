#include "UserRepository.h"
#include "DictMapping.h"

#include <utility>

UserRepository::UserRepository(std::shared_ptr<DictDbConnection> conn)
    : m_conn(std::move(conn))
{
}

void UserRepository::addHistory(int entryId)
{
    DictDb *handle = db();
    if (handle == nullptr) return;
    dict_db_add_history(handle, entryId);
}

QVector<Entry> UserRepository::recentHistory(int limit) const
{
    DictDb *handle = db();
    if (handle == nullptr) return {};

    DictEntryList list = {};
    if (dict_db_list_history(handle, limit, &list) != DICT_OK)
        return {};
    return mapping::drainEntries(&list);
}

bool UserRepository::toggleFavorite(int entryId)
{
    DictDb *handle = db();
    if (handle == nullptr) return false;

    int fav = 0;
    dict_db_toggle_favorite(handle, entryId, &fav);
    return fav != 0;
}

bool UserRepository::isFavorite(int entryId) const
{
    DictDb *handle = db();
    if (handle == nullptr) return false;

    int fav = 0;
    dict_db_is_favorite(handle, entryId, &fav);
    return fav != 0;
}

QVector<Entry> UserRepository::favorites(int limit) const
{
    DictDb *handle = db();
    if (handle == nullptr) return {};

    if (limit <= 0) limit = -1;

    DictEntryList list = {};
    if (dict_db_list_favorites(handle, limit, &list) != DICT_OK)
        return {};
    return mapping::drainEntries(&list);
}

QVector<Note> UserRepository::notesFor(int entryId) const
{
    DictDb *handle = db();
    if (handle == nullptr) return {};

    DictNoteList list = {};
    if (dict_db_list_notes(handle, entryId, &list) != DICT_OK)
        return {};
    return mapping::drainNotes(&list);
}

int UserRepository::addNote(const Note &note)
{
    DictDb *handle = db();
    if (handle == nullptr) return 0;

    DictNote c = mapping::fromNote(note);
    int newId = 0;
    if (dict_db_add_note(handle, &c, &newId) != DICT_OK)
        return 0;
    return newId;
}

bool UserRepository::updateNote(const Note &note)
{
    DictDb *handle = db();
    if (handle == nullptr) return false;

    DictNote c = mapping::fromNote(note);
    return dict_db_update_note(handle, &c) == DICT_OK;
}

bool UserRepository::deleteNote(int noteId)
{
    DictDb *handle = db();
    if (handle == nullptr) return false;
    return dict_db_delete_note(handle, noteId) == DICT_OK;
}

QHash<int, QVector<Note>> UserRepository::notesForFavorites(int limit) const
{
    QHash<int, QVector<Note>> byEntry;

    DictDb *handle = db();
    if (handle == nullptr) return byEntry;

    if (limit <= 0) limit = -1;

    DictNoteList list = {};
    if (dict_db_list_favorite_notes(handle, limit, &list) != DICT_OK)
        return byEntry;

    for (const Note &n : mapping::drainNotes(&list))
        byEntry[n.entryId].append(n);

    return byEntry;
}
