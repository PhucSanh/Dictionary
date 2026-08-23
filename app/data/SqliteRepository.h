#pragma once

#include "DbPaths.h"
#include "domain/IDictionaryRepository.h"
#include "domain/IUserDataRepository.h"

extern "C" {
#include "dict_db.h"
}


class SqliteRepository final : public IDictionaryRepository,
                               public IUserDataRepository {
public:
    explicit SqliteRepository(const DbPaths &paths);
    ~SqliteRepository() override;

    SqliteRepository(const SqliteRepository &)            = delete;
    SqliteRepository &operator=(const SqliteRepository &) = delete;

    QString lastError() const;

    bool           isOpen() const override;
    QVector<Entry> searchByWord(const QString &query, int limit, int offset) const override;
    QVector<Entry> searchByMeaning(const QString &query, int limit, int offset) const override;
    int            countMatches(const QString &query) const override;

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
    DictDb *m_db = nullptr;
};
