#include "ViewModel/EntryModel.h"
#include "ViewModel/SpeechViewModel.h"

#include "Model/Repositories/CoreTextAnalyzer.h"
#include "Model/Database/DbPaths.h"
#include "Model/Database/DictDbConnection.h"
#include "Model/Repositories/DictionaryRepository.h"
#include "Model/Repositories/UserRepository.h"
#include "Model/Services/QtSpeechService.h"

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
    auto speechService  = std::make_shared<QtSpeechService>();

    EntryModel      entryModel(dictionaryRepo, userRepo, textAnalyzer);
    SpeechViewModel speechViewModel(speechService);

    if (!speechService->isAvailable()) {
        qWarning() << "Khong dung duoc chuc nang phat am:"
                   << "thieu module Qt TextToSpeech hoac giong tieng Nhat cua he thong.";
    }

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.setInitialProperties({
        { QStringLiteral("entryModel"),      QVariant::fromValue(&entryModel) },
        { QStringLiteral("speechViewModel"), QVariant::fromValue(&speechViewModel) },
    });

    engine.loadFromModule("Dictionary", "Main");

    return QGuiApplication::exec();
}
