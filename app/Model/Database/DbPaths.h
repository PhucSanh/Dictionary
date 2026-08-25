#pragma once
#include <QString>

struct DbPaths {
    QString dictionaryDb;
    QString userDb;

    static DbPaths defaults();

    bool isValid() const {
        return !dictionaryDb.isEmpty() && !userDb.isEmpty();
    }
};
