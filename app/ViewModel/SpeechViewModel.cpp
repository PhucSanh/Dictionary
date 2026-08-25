#include "ViewModel/SpeechViewModel.h"

#include <utility>

SpeechViewModel::SpeechViewModel(std::shared_ptr<ISpeechService> speech,
                                 QObject *parent)
    : QObject(parent)
    , m_speech(std::move(speech))
{
    if (!m_speech)
        return;

    m_speech->setSpeakingChangedCallback([this](bool speaking) {
        if (m_speaking == speaking)
            return;
        m_speaking = speaking;
        emit speakingChanged();
    });
}

SpeechViewModel::~SpeechViewModel()
{
    if (m_speech)
        m_speech->setSpeakingChangedCallback(nullptr);
}

bool SpeechViewModel::available() const
{
    return m_speech && m_speech->isAvailable();
}

bool SpeechViewModel::speaking() const
{
    return m_speaking;
}

void SpeechViewModel::speak(const QString &text)
{
    if (m_speech)
        m_speech->speak(text);
}

void SpeechViewModel::speakEntry(const QString &readingHira, const QString &word)
{
    const QString reading = readingHira.trimmed();
    speak(reading.isEmpty() ? word : reading);
}

void SpeechViewModel::stop()
{
    if (m_speech)
        m_speech->stop();
}
