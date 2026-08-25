#pragma once
#include "Model/Entities/Entry.h"
#include <QString>
#include <QVector>

class IDictionaryRepository {
public:
    virtual ~IDictionaryRepository() = default;

    virtual bool isOpen() const = 0;

    virtual QVector<Entry> searchByWord(const QString &query,
                                        int limit, int offset) const = 0;

    virtual QVector<Entry> searchByMeaning(const QString &query,
                                           int limit, int offset) const = 0;

    virtual int countByWord(const QString &query) const = 0;
    virtual int countByMeaning(const QString &query) const = 0;
};
