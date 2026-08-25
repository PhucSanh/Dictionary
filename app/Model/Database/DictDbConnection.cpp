#include "Model/Database/DictDbConnection.h"

#include <QDebug>

DictDbConnection::DictDbConnection(const DbPaths &paths)
{
    if (!paths.isValid()) {
        qWarning() << "[DictDbConnection] Duong dan database khong hop le";
        return;
    }

    const QByteArray dictPath = paths.dictionaryDb.toUtf8();
    if (dict_db_open(dictPath.constData(), &m_db) != DICT_OK) {
        qWarning() << "[DictDbConnection] Khong mo duoc tu dien:" << paths.dictionaryDb;
        m_db = nullptr;
        return;
    }

    const QByteArray userPath = paths.userDb.toUtf8();
    if (dict_db_attach_user(m_db, userPath.constData()) != DICT_OK) {
        qWarning() << "[DictDbConnection] ATTACH user.db that bai:" << lastError();
    }
}

DictDbConnection::~DictDbConnection()
{
    if (m_db != nullptr) {
        dict_db_close(m_db);
        m_db = nullptr;
    }
}

QString DictDbConnection::lastError() const
{
    return m_db ? QString::fromUtf8(dict_db_last_error(m_db))
                : QStringLiteral("db chua mo");
}
