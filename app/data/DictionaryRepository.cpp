#include "DictionaryRepository.h"
#include "DictMapping.h"

#include <utility>

DictionaryRepository::DictionaryRepository(std::shared_ptr<DictDbConnection> conn)
    : m_conn(std::move(conn))
{
}

bool DictionaryRepository::isOpen() const
{
    return db() != nullptr;
}

QVector<Entry> DictionaryRepository::searchByWord(const QString &query,
                                                  int limit, int offset) const
{
    DictDb *handle = db();
    if (handle == nullptr) return {};

    const QByteArray q = query.toUtf8();
    DictEntryList list = {};
    if (dict_db_search(handle, q.constData(), limit, offset, &list) != DICT_OK)
        return {};
    return mapping::drainEntries(&list);
}

QVector<Entry> DictionaryRepository::searchByMeaning(const QString &query,
                                                     int limit, int offset) const
{
    DictDb *handle = db();
    if (handle == nullptr) return {};

    const QByteArray q = query.toUtf8();
    DictEntryList list = {};
    if (dict_db_search_meaning(handle, q.constData(), limit, offset, &list) != DICT_OK)
        return {};
    return mapping::drainEntries(&list);
}

namespace {

int countWith(DictDb *handle, const QString &query, bool isMeaning)
{
    if (handle == nullptr) return 0;

    const QByteArray q = query.toUtf8();
    int total = 0;
    dict_db_count(handle, q.constData(), isMeaning ? 1 : 0, &total);
    return total;
}

}

int DictionaryRepository::countByWord(const QString &query) const
{
    return countWith(db(), query, false);
}

int DictionaryRepository::countByMeaning(const QString &query) const
{
    return countWith(db(), query, true);
}
