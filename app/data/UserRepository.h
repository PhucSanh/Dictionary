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

    QVector<Note>  notesFor(int entryId) const override;
    int            addNote(const Note &note) override;
    bool           updateNote(const Note &note) override;
    bool           deleteNote(int noteId) override;

    QHash<int, QVector<Note>> notesForFavorites(int limit) const override;

private:
    DictDb *db() const { return m_conn ? m_conn->handle() : nullptr; }

    std::shared_ptr<DictDbConnection> m_conn;
};
