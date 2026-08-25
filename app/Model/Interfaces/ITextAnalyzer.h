#pragma once
#include "Model/Entities/ConjugationForm.h"
#include <QString>
#include <QStringList>
#include <QVector>

enum class InputKind {
    Unknown,
    Japanese,
    Latin,
    Vietnamese
};

class ITextAnalyzer {
public:
    virtual ~ITextAnalyzer() = default;

    virtual InputKind detect(const QString &text) const = 0;
    virtual QString   toHiragana(const QString &text) const = 0;

    virtual QStringList deinflect(const QString &text) const = 0;

    virtual QVector<ConjugationForm> conjugate(const QString &word,
                                               const QString &readingHira,
                                               const QString &partOfSpeech) const = 0;
};
