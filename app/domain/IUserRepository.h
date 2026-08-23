#pragma once
#include "Entry.h"
#include "Note.h"
#include <QHash>
#include <QVector>

class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual void           addHistory(int entryId) = 0;
    virtual QVector<Entry> recentHistory(int limit) const = 0;

    virtual bool           toggleFavorite(int entryId) = 0;
    virtual bool           isFavorite(int entryId) const = 0;
    virtual QVector<Entry> favorites(int limit) const = 0;

    virtual QVector<Note>  notesFor(int entryId) const = 0;
    virtual int            addNote(const Note &note) = 0;
    virtual bool           updateNote(const Note &note) = 0;
    virtual bool           deleteNote(int noteId) = 0;

    virtual QHash<int, QVector<Note>> notesForFavorites(int limit) const = 0;
};
