#pragma once
#include <QString>

struct Entry {
    int id = 0;
    QString word;
    QString reading;
    QString readingHira;
    QString romaji;
    QString partOfSpeech;
    QString meaning;
    QString english;
    QString level;
};