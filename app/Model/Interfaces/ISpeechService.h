#pragma once

#include <QString>
#include <functional>

class ISpeechService {
public:
    virtual ~ISpeechService() = default;

    virtual bool isAvailable() const = 0;
    virtual bool isSpeaking() const = 0;

    virtual void speak(const QString &text) = 0;
    virtual void stop() = 0;

    virtual void setSpeakingChangedCallback(std::function<void(bool)> callback) = 0;
};
