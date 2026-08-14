#pragma once
#include <QSqlDatabase>
#include <QString>
#include <QVector>
#include "entry.h"

class Dictionary
{
public:
    explicit Dictionary(const QString &dbPath);
    ~Dictionary();

    bool isOpen() const;
    QString lastError() const;

    QVector<Entry> search(const QString &query, int limit = 10) const;
    QVector<Entry> searchByMeaning(const QString &query, int limit = 10) const;

private:
    QSqlDatabase m_db;
    QString m_error;
};