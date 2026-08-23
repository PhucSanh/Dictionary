#pragma once
#include "Entry.h"
#include "Note.h"
#include <QVector>

struct Flashcard {
    Entry         entry;
    QVector<Note> notes;
};
