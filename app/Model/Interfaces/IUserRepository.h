#pragma once
#include "Model/Entities/Category.h"
#include "Model/Entities/Entry.h"
#include "Model/Entities/Note.h"
#include <QHash>
#include <QVector>

class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual void           addHistory(int entryId) = 0;
    virtual QVector<Entry> recentHistory(int limit, int offset) const = 0;
    virtual int            historyCount() const = 0;

    virtual bool           toggleFavorite(int entryId) = 0;
    virtual bool           isFavorite(int entryId) const = 0;
    virtual QVector<Entry> favorites(int limit) const = 0;

    virtual QVector<Category> categories() const = 0;
    virtual int               addCategory(const QString &name) = 0;
    virtual bool              renameCategory(int categoryId, const QString &name) = 0;
    virtual bool              deleteCategory(int categoryId) = 0;

    virtual bool              addFavorite(int entryId, const QVector<int> &categoryIds) = 0;
    virtual bool              removeFavorite(int entryId) = 0;
    virtual bool              setFavoriteCategories(int entryId, const QVector<int> &categoryIds) = 0;
    virtual QVector<Category> categoriesFor(int entryId) const = 0;
    virtual QVector<Entry>    favoritesInCategory(int categoryId, int limit, int offset) const = 0;
    virtual int               favoriteCount(int categoryId) const = 0;

    virtual QVector<Note>  notesFor(int entryId) const = 0;
    virtual int            addNote(const Note &note) = 0;
    virtual bool           updateNote(const Note &note) = 0;
    virtual bool           deleteNote(int noteId) = 0;

    virtual QHash<int, QVector<Note>> notesForFavorites(int limit) const = 0;
    virtual QHash<int, QVector<Note>> notesForFavoritesInCategory(int categoryId, int limit) const = 0;
};
