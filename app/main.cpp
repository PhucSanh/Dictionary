#include "entrymodel.h"

#include "data/CoreTextAnalyzer.h"
#include "data/DbPaths.h"
#include "data/DictDbConnection.h"
#include "data/DictionaryRepository.h"
#include "data/UserRepository.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QVariant>
#include <QDebug>

#include <memory>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Slazenger");
    QCoreApplication::setApplicationName("Dictionary");

    const DbPaths paths = DbPaths::defaults();

    auto connection = std::make_shared<DictDbConnection>(paths);
    if (!connection->isOpen()) {
        qCritical() << "Khong mo duoc tu dien tai" << paths.dictionaryDb
                    << "- app se chay nhung khong tra cuu duoc gi.";
    }

    auto dictionaryRepo = std::make_shared<DictionaryRepository>(connection);
    auto userRepo       = std::make_shared<UserRepository>(connection);
    auto textAnalyzer   = std::make_shared<CoreTextAnalyzer>();

    EntryModel entryModel(dictionaryRepo, userRepo, textAnalyzer);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.setInitialProperties({
        { QStringLiteral("entryModel"), QVariant::fromValue(&entryModel) },
    });

    engine.loadFromModule("Dictionary", "Main");

    return QGuiApplication::exec();
}
