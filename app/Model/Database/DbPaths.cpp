#include "Model/Database/DbPaths.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

DbPaths DbPaths::defaults()
{
    DbPaths paths;
    paths.dictionaryDb = QCoreApplication::applicationDirPath() + "/dictionary.db";

    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dir.isEmpty()) {
        qWarning() << "[DbPaths] Khong xac dinh duoc AppDataLocation";
        return paths;
    }

    if (!QDir().mkpath(dir)) {
        qWarning() << "[DbPaths] Khong tao duoc thu muc du lieu:" << dir;
        return paths;
    }

    paths.userDb = dir + "/user.db";
    return paths;
}
