#pragma once

#include "Model/Entities/Category.h"
#include "Model/Entities/Entry.h"
#include "Model/Entities/Note.h"
#include <QVector>

extern "C" {
#include "dict_types.h"
}

namespace mapping {

Entry    toEntry(const DictEntry &c);
Note     toNote(const DictNote &c);
Category toCategory(const DictCategory &c);
DictNote fromNote(const Note &n);

QVector<Entry>    drainEntries(DictEntryList *list);
QVector<Note>     drainNotes(DictNoteList *list);
QVector<Category> drainCategories(DictCategoryList *list);

}
