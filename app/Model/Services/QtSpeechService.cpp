#include "Model/Services/QtSpeechService.h"

#include <utility>

#ifdef DICT_HAS_TTS
#include <QList>
#include <QLocale>
#include <QTextToSpeech>
#include <QVoice>
#endif

QtSpeechService::QtSpeechService(QObject *parent)
    : QObject(parent)
{
#ifdef DICT_HAS_TTS
    m_tts = new QTextToSpeech(this);

    if (m_tts->state() == QTextToSpeech::Error)
        return;

    bool hasJapanese = false;
    const QList<QLocale> locales = m_tts->availableLocales();
    for (const QLocale &locale : locales) {
        if (locale.language() != QLocale::Japanese)
            continue;
        m_tts->setLocale(locale);
        hasJapanese = true;
        break;
    }

    if (!hasJapanese)
        return;

    const QList<QVoice> voices = m_tts->availableVoices();
    if (voices.isEmpty())
        return;

    m_tts->setVoice(voices.first());
    m_available = true;

    connect(m_tts, &QTextToSpeech::stateChanged, this,
            [this](QTextToSpeech::State state) {
                setSpeaking(state == QTextToSpeech::Speaking
                            || state == QTextToSpeech::Synthesizing);
            });
#endif
}

QtSpeechService::~QtSpeechService()
{
#ifdef DICT_HAS_TTS
    if (m_tts != nullptr)
        m_tts->stop();
#endif
}

bool QtSpeechService::isAvailable() const { return m_available; }
bool QtSpeechService::isSpeaking() const  { return m_speaking; }

void QtSpeechService::speak(const QString &text)
{
#ifdef DICT_HAS_TTS
    if (!m_available || m_tts == nullptr)
        return;

    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty())
        return;

    m_tts->stop();
    m_tts->say(trimmed);
#else
    Q_UNUSED(text)
#endif
}

void QtSpeechService::stop()
{
#ifdef DICT_HAS_TTS
    if (m_tts != nullptr)
        m_tts->stop();
#endif
}

void QtSpeechService::setSpeakingChangedCallback(std::function<void(bool)> callback)
{
    m_onSpeakingChanged = std::move(callback);
}

void QtSpeechService::setSpeaking(bool speaking)
{
    if (m_speaking == speaking)
        return;

    m_speaking = speaking;
    if (m_onSpeakingChanged)
        m_onSpeakingChanged(speaking);
}
