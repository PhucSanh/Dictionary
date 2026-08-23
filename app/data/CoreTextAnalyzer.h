#pragma once
#include "domain/ITextAnalyzer.h"

class CoreTextAnalyzer final : public ITextAnalyzer {
public:
    InputKind detect(const QString &text) const override;
    QString   toHiragana(const QString &text) const override;

    QStringList deinflect(const QString &text) const override;

    QVector<ConjugationForm> conjugate(const QString &word,
                                       const QString &readingHira,
                                       const QString &partOfSpeech) const override;
};
