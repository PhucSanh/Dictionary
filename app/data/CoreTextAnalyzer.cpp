#include "CoreTextAnalyzer.h"

extern "C" {
#include "dict_conjugate.h"
#include "dict_deinflect.h"
#include "dict_kana.h"
#include "dict_types.h"
}

InputKind CoreTextAnalyzer::detect(const QString &text) const
{
    const QByteArray raw = text.toUtf8();
    switch (dict_kana_detect(raw.constData())) {
    case DICT_INPUT_JAPANESE:   return InputKind::Japanese;
    case DICT_INPUT_LATIN:      return InputKind::Latin;
    case DICT_INPUT_VIETNAMESE: return InputKind::Vietnamese;
    default:                    return InputKind::Unknown;
    }
}

QString CoreTextAnalyzer::toHiragana(const QString &text) const
{
    const QByteArray raw = text.toUtf8();
    char hira[DICT_READING_LEN];
    if (dict_kana_to_hiragana(raw.constData(), hira, sizeof hira) != DICT_OK)
        return text;
    return QString::fromUtf8(hira);
}

QStringList CoreTextAnalyzer::deinflect(const QString &text) const
{
    const QByteArray raw = text.toUtf8();

    char candidates[DICT_MAX_CANDIDATES][DICT_WORD_LEN];
    const int n = dict_deinflect(raw.constData(), candidates, DICT_MAX_CANDIDATES);

    QStringList out;
    out.reserve(n);
    for (int i = 0; i < n; ++i)
        out.append(QString::fromUtf8(candidates[i]));
    return out;
}

QVector<ConjugationForm> CoreTextAnalyzer::conjugate(const QString &word,
                                                     const QString &readingHira,
                                                     const QString &partOfSpeech) const
{
    const QByteArray w = word.toUtf8();
    const QByteArray r = readingHira.toUtf8();
    const QByteArray p = partOfSpeech.toUtf8();

    DictForm forms[DICT_MAX_FORMS];
    const int n = dict_conjugate(w.constData(), r.constData(), p.constData(),
                                 forms, DICT_MAX_FORMS);

    QVector<ConjugationForm> out;
    out.reserve(n);
    for (int i = 0; i < n; ++i) {
        ConjugationForm f;
        f.name = QString::fromUtf8(forms[i].name);
        f.text = QString::fromUtf8(forms[i].text);
        out.append(f);
    }
    return out;
}
