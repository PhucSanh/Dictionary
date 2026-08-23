#pragma once

#include "DictDbConnection.h"
#include "domain/IDictionaryRepository.h"

#include <memory>

class DictionaryRepository final : public IDictionaryRepository {
public:
    explicit DictionaryRepository(std::shared_ptr<DictDbConnection> conn);

    bool           isOpen() const override;
    QVector<Entry> searchByWord(const QString &query, int limit, int offset) const override;
    QVector<Entry> searchByMeaning(const QString &query, int limit, int offset) const override;
    int            countByWord(const QString &query) const override;
    int            countByMeaning(const QString &query) const override;

private:
    DictDb *db() const { return m_conn ? m_conn->handle() : nullptr; }

    std::shared_ptr<DictDbConnection> m_conn;
};
