#pragma once
#include "Model/Entities/Entry.h"
#include <QVector>

struct SearchResult {
    QVector<Entry> entries;
    int            totalCount = 0;

    QString deinflectedFrom;
    QString deinflectedTo;

    bool isEmpty() const { return entries.isEmpty(); }
};
