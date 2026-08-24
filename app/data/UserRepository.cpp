#include "UserRepository.h"
#include "DictMapping.h"

#include <QStringList>
#include <utility>

UserRepository::UserRepository(std::shared_ptr<DictDbConnection> conn)
    : m_conn(std::move(conn))
{
    seedDefaultCategories();
}

void UserRepository::seedDefaultCategories()
{
    if (db() == nullptr || !categories().isEmpty())
        return;

    const QStringList defaults = {
        QStringLiteral("N5"),
        QStringLiteral("N4"),
        QStringLiteral("N3"),
        QStringLiteral("N2"),
        QStringLiteral("N1"),
        QStringLiteral("Tiếng Nhật IT"),
        QStringLiteral("Khác"),
    };
    for (const QString &name : defaults)
        addCategory(name);
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
    return favoritesInCategory(0, limit);
}

QVector<Entry> UserRepository::favoritesInCategory(int categoryId, int limit) const
{
    DictDb *handle = db();
    if (handle == nullptr) return {};

    if (limit <= 0) limit = -1;

    DictEntryList list = {};
    if (dict_db_list_favorites_in_category(handle, categoryId, limit, &list) != DICT_OK)
        return {};
    return mapping::drainEntries(&list);
}

int UserRepository::favoriteCount(int categoryId) const
{
    DictDb *handle = db();
    if (handle == nullptr) return 0;

    int total = 0;
    if (dict_db_count_favorites(handle, categoryId, &total) != DICT_OK)
        return 0;
    return total;
}

QVector<Category> UserRepository::categories() const
{
    DictDb *handle = db();
    if (handle == nullptr) return {};

    DictCategoryList list = {};
    if (dict_db_list_categories(handle, &list) != DICT_OK)
        return {};
    return mapping::drainCategories(&list);
}

int UserRepository::addCategory(const QString &name)
{
    DictDb *handle = db();
    if (handle == nullptr) return 0;

    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty()) return 0;

    const QByteArray utf8 = trimmed.toUtf8();
    int newId = 0;
    if (dict_db_add_category(handle, utf8.constData(), &newId) != DICT_OK)
        return 0;
    return newId;
}

bool UserRepository::renameCategory(int categoryId, const QString &name)
{
    DictDb *handle = db();
    if (handle == nullptr) return false;

    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty()) return false;

    const QByteArray utf8 = trimmed.toUtf8();
    return dict_db_rename_category(handle, categoryId, utf8.constData()) == DICT_OK;
}

bool UserRepository::deleteCategory(int categoryId)
{
    DictDb *handle = db();
    if (handle == nullptr) return false;
    return dict_db_delete_category(handle, categoryId) == DICT_OK;
}

bool UserRepository::addFavorite(int entryId, const QVector<int> &categoryIds)
{
    DictDb *handle = db();
    if (handle == nullptr) return false;

    return dict_db_add_favorite(handle, entryId,
                                categoryIds.constData(),
                                static_cast<int>(categoryIds.size())) == DICT_OK;
}

bool UserRepository::removeFavorite(int entryId)
{
    DictDb *handle = db();
    if (handle == nullptr) return false;
    return dict_db_remove_favorite(handle, entryId) == DICT_OK;
}

bool UserRepository::setFavoriteCategories(int entryId, const QVector<int> &categoryIds)
{
    DictDb *handle = db();
    if (handle == nullptr) return false;

    return dict_db_set_favorite_categories(handle, entryId,
                                           categoryIds.constData(),
                                           static_cast<int>(categoryIds.size())) == DICT_OK;
}

QVector<Category> UserRepository::categoriesFor(int entryId) const
{
    DictDb *handle = db();
    if (handle == nullptr) return {};

    DictCategoryList list = {};
    if (dict_db_list_entry_categories(handle, entryId, &list) != DICT_OK)
        return {};
    return mapping::drainCategories(&list);
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
    return notesForFavoritesInCategory(0, limit);
}

QHash<int, QVector<Note>> UserRepository::notesForFavoritesInCategory(int categoryId, int limit) const
{
    QHash<int, QVector<Note>> byEntry;

    DictDb *handle = db();
    if (handle == nullptr) return byEntry;

    if (limit <= 0) limit = -1;

    DictNoteList list = {};
    if (dict_db_list_favorite_notes_in_category(handle, categoryId, limit, &list) != DICT_OK)
        return byEntry;

    for (const Note &n : mapping::drainNotes(&list))
        byEntry[n.entryId].append(n);

    return byEntry;
}
