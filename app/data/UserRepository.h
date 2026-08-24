#pragma once

#include "DictDbConnection.h"
#include "domain/IUserRepository.h"

#include <memory>

class UserRepository final : public IUserRepository {
public:
    explicit UserRepository(std::shared_ptr<DictDbConnection> conn);

    void           addHistory(int entryId) override;
    QVector<Entry> recentHistory(int limit) const override;

    bool           toggleFavorite(int entryId) override;
    bool           isFavorite(int entryId) const override;
    QVector<Entry> favorites(int limit) const override;

    QVector<Category> categories() const override;
    int               addCategory(const QString &name) override;
    bool              renameCategory(int categoryId, const QString &name) override;
    bool              deleteCategory(int categoryId) override;

    bool              addFavorite(int entryId, const QVector<int> &categoryIds) override;
    bool              removeFavorite(int entryId) override;
    bool              setFavoriteCategories(int entryId, const QVector<int> &categoryIds) override;
    QVector<Category> categoriesFor(int entryId) const override;
    QVector<Entry>    favoritesInCategory(int categoryId, int limit) const override;
    int               favoriteCount(int categoryId) const override;

    QVector<Note>  notesFor(int entryId) const override;
    int            addNote(const Note &note) override;
    bool           updateNote(const Note &note) override;
    bool           deleteNote(int noteId) override;

    QHash<int, QVector<Note>> notesForFavorites(int limit) const override;
    QHash<int, QVector<Note>> notesForFavoritesInCategory(int categoryId, int limit) const override;

private:
    DictDb *db() const { return m_conn ? m_conn->handle() : nullptr; }

    void seedDefaultCategories();

    std::shared_ptr<DictDbConnection> m_conn;
};
