#pragma once

#include "domain/Entry.h"
#include "domain/Note.h"
#include <QVector>

extern "C" {
#include "dict_types.h"
}

namespace mapping {

Entry    toEntry(const DictEntry &c);
Note     toNote(const DictNote &c);
DictNote fromNote(const Note &n);

QVector<Entry> drainEntries(DictEntryList *list);
QVector<Note>  drainNotes(DictNoteList *list);

}
