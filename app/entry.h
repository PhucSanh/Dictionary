#ifndef ENTRY_H
#define ENTRY_H

#include <QString>

struct Entry {
    int id = 0;
    QString word, reading, readingHira, romaji;
    QString partOfSpeech, meaning, english, level;
};
#endif // ENTRY_H
