#pragma once
#include "Model/Entities/Entry.h"
#include "Model/Entities/Note.h"
#include <QVector>

struct Flashcard {
    Entry         entry;
    QVector<Note> notes;
};
