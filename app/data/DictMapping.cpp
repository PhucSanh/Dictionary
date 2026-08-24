#include "DictMapping.h"

#include <cstdio>
#include <cstring>

namespace mapping {

Entry toEntry(const DictEntry &c)
{
    Entry e;
    e.id           = c.id;
    e.word         = QString::fromUtf8(c.word);
    e.reading      = QString::fromUtf8(c.reading);
    e.readingHira  = QString::fromUtf8(c.reading_hira);
    e.romaji       = QString::fromUtf8(c.romaji);
    e.partOfSpeech = QString::fromUtf8(c.part_of_speech);
    e.meaning      = QString::fromUtf8(c.meaning);
    e.english      = QString::fromUtf8(c.english);
    e.level        = QString::fromUtf8(c.level);
    e.categories   = QString::fromUtf8(c.categories);
    return e;
}

Note toNote(const DictNote &c)
{
    Note n;
    n.id          = c.id;
    n.entryId     = c.entry_id;
    n.japanese    = QString::fromUtf8(c.japanese);
    n.translation = QString::fromUtf8(c.translation);
    n.note        = QString::fromUtf8(c.note);
    return n;
}

Category toCategory(const DictCategory &c)
{
    Category out;
    out.id         = c.id;
    out.name       = QString::fromUtf8(c.name);
    out.entryCount = c.entry_count;
    return out;
}

DictNote fromNote(const Note &n)
{
    DictNote c;
    std::memset(&c, 0, sizeof c);
    c.id       = n.id;
    c.entry_id = n.entryId;

    const QByteArray jp = n.japanese.toUtf8();
    const QByteArray tr = n.translation.toUtf8();
    const QByteArray nt = n.note.toUtf8();
    std::snprintf(c.japanese,    sizeof c.japanese,    "%s", jp.constData());
    std::snprintf(c.translation, sizeof c.translation, "%s", tr.constData());
    std::snprintf(c.note,        sizeof c.note,        "%s", nt.constData());
    return c;
}

QVector<Entry> drainEntries(DictEntryList *list)
{
    QVector<Entry> out;
    out.reserve(list->count);
    for (int i = 0; i < list->count; ++i)
        out.append(toEntry(list->items[i]));
    dict_entry_list_free(list);
    return out;
}

QVector<Note> drainNotes(DictNoteList *list)
{
    QVector<Note> out;
    out.reserve(list->count);
    for (int i = 0; i < list->count; ++i)
        out.append(toNote(list->items[i]));
    dict_note_list_free(list);
    return out;
}

QVector<Category> drainCategories(DictCategoryList *list)
{
    QVector<Category> out;
    out.reserve(list->count);
    for (int i = 0; i < list->count; ++i)
        out.append(toCategory(list->items[i]));
    dict_category_list_free(list);
    return out;
}

}
