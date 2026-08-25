#pragma once
#include <QString>

struct Note {
    int     id      = 0;
    int     entryId = 0;
    QString japanese;
    QString translation;
    QString note;

    bool isEmpty() const {
        return japanese.isEmpty() && translation.isEmpty() && note.isEmpty();
    }
};
