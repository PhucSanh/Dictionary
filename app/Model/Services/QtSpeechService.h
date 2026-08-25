#pragma once

#include "Model/Interfaces/ISpeechService.h"

#include <QObject>
#include <QString>
#include <functional>

QT_BEGIN_NAMESPACE
class QTextToSpeech;
QT_END_NAMESPACE

class QtSpeechService final : public QObject, public ISpeechService {
    Q_OBJECT

public:
    explicit QtSpeechService(QObject *parent = nullptr);
    ~QtSpeechService() override;

    bool isAvailable() const override;
    bool isSpeaking() const override;

    void speak(const QString &text) override;
    void stop() override;

    void setSpeakingChangedCallback(std::function<void(bool)> callback) override;

private:
    void setSpeaking(bool speaking);

    QTextToSpeech *m_tts       = nullptr;
    bool           m_available = false;
    bool           m_speaking  = false;

    std::function<void(bool)> m_onSpeakingChanged;
};
