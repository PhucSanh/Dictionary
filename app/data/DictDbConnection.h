#pragma once

#include "DbPaths.h"
#include <QString>

extern "C" {
#include "dict_db.h"
}

class DictDbConnection {
public:
    explicit DictDbConnection(const DbPaths &paths);
    ~DictDbConnection();

    DictDbConnection(const DictDbConnection &)            = delete;
    DictDbConnection &operator=(const DictDbConnection &) = delete;

    bool    isOpen() const { return m_db != nullptr; }
    DictDb *handle() const { return m_db; }
    QString lastError() const;

private:
    DictDb *m_db = nullptr;
};
