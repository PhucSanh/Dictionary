#pragma once

#include "Model/Interfaces/ISpeechService.h"

#include <QObject>
#include <QString>
#include <QtQml/qqmlregistration.h>

#include <memory>

class SpeechViewModel : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("SpeechViewModel duoc tao o main.cpp va tiem vao qua Main.qml")

    Q_PROPERTY(bool available READ available CONSTANT)
    Q_PROPERTY(bool speaking READ speaking NOTIFY speakingChanged)

public:
    explicit SpeechViewModel(std::shared_ptr<ISpeechService> speech,
                             QObject *parent = nullptr);
    ~SpeechViewModel() override;

    bool available() const;
    bool speaking() const;

    Q_INVOKABLE void speak(const QString &text);
    Q_INVOKABLE void speakEntry(const QString &readingHira, const QString &word);
    Q_INVOKABLE void stop();

signals:
    void speakingChanged();

private:
    std::shared_ptr<ISpeechService> m_speech;
    bool m_speaking = false;
};
