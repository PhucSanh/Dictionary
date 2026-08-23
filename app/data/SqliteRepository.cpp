#include "SqliteRepository.h"

#include <QDebug>
#include <cstdio>
#include <cstring>

namespace {


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

// Gom mot DictEntryList thanh QVector<Entry> roi giai phong ban goc.
QVector<Entry> drainEntries(DictEntryList *list)
{
    QVector<Entry> out;
    out.reserve(list->count);
    for (int i = 0; i < list->count; ++i)
        out.append(toEntry(list->items[i]));
    dict_entry_list_free(list);
    return out;
}

} // namespace

SqliteRepository::SqliteRepository(const DbPaths &paths)
{
    if (!paths.isValid()) {
        qWarning() << "[SqliteRepository] Duong dan database khong hop le";
        return;
    }

    const QByteArray dictPath = paths.dictionaryDb.toUtf8();
    if (dict_db_open(dictPath.constData(), &m_db) != DICT_OK) {
        qWarning() << "[SqliteRepository] Khong mo duoc tu dien:" << paths.dictionaryDb;
        m_db = nullptr;
        return;
    }

    const QByteArray userPath = paths.userDb.toUtf8();
    if (dict_db_attach_user(m_db, userPath.constData()) != DICT_OK) {
        qWarning() << "[SqliteRepository] ATTACH user.db that bai:" << lastError();
        // Van dung duoc phan tra cuu, chi mat yeu thich / ghi chu.
    }
}

SqliteRepository::~SqliteRepository()
{
    if (m_db != nullptr) {
        dict_db_close(m_db);
        m_db = nullptr;
    }
}

QString SqliteRepository::lastError() const
{
    return m_db ? QString::fromUtf8(dict_db_last_error(m_db)) : QStringLiteral("db chua mo");
}

bool SqliteRepository::isOpen() const
{
    return m_db != nullptr;
}

// ---------------------------------------------------------------- tu dien

QVector<Entry> SqliteRepository::searchByWord(const QString &query,
                                              int limit, int offset) const
{
    if (m_db == nullptr) return {};

    const QByteArray q = query.toUtf8();
    DictEntryList list = {};
    if (dict_db_search(m_db, q.constData(), limit, offset, &list) != DICT_OK)
        return {};
    return drainEntries(&list);
}

QVector<Entry> SqliteRepository::searchByMeaning(const QString &query,
                                                 int limit, int offset) const
{
    if (m_db == nullptr) return {};

    const QByteArray q = query.toUtf8();
    DictEntryList list = {};
    if (dict_db_search_meaning(m_db, q.constData(), limit, offset, &list) != DICT_OK)
        return {};
    return drainEntries(&list);
}

int SqliteRepository::countMatches(const QString &query) const
{
    if (m_db == nullptr) return 0;

    const QByteArray q = query.toUtf8();
    int total = 0;
    dict_db_count_total(m_db, q.constData(), &total);
    return total;
}


void SqliteRepository::addHistory(int entryId)
{
    if (m_db == nullptr) return;
    dict_db_add_history(m_db, entryId);
}

QVector<Entry> SqliteRepository::recentHistory(int limit) const
{
    if (m_db == nullptr) return {};

    DictEntryList list = {};
    if (dict_db_list_history(m_db, limit, &list) != DICT_OK)
        return {};
    return drainEntries(&list);
}

// ----------------------------------------------------------- yeu thich

bool SqliteRepository::toggleFavorite(int entryId)
{
    if (m_db == nullptr) return false;

    int fav = 0;
    dict_db_toggle_favorite(m_db, entryId, &fav);
    return fav != 0;
}

bool SqliteRepository::isFavorite(int entryId) const
{
    if (m_db == nullptr) return false;

    int fav = 0;
    dict_db_is_favorite(m_db, entryId, &fav);
    return fav != 0;
}

QVector<Entry> SqliteRepository::favorites(int limit) const
{
    if (m_db == nullptr) return {};

    // limit <= 0 -> -1 -> SQLite hieu la khong gioi han.
    if (limit <= 0) limit = -1;

    DictEntryList list = {};
    if (dict_db_list_favorites(m_db, limit, &list) != DICT_OK)
        return {};
    return drainEntries(&list);
}

// -------------------------------------------------------------- ghi chu

QVector<Note> SqliteRepository::notesFor(int entryId) const
{
    if (m_db == nullptr) return {};

    DictNoteList list = {};
    if (dict_db_list_notes(m_db, entryId, &list) != DICT_OK)
        return {};

    QVector<Note> out;
    out.reserve(list.count);
    for (int i = 0; i < list.count; ++i)
        out.append(toNote(list.items[i]));

    dict_note_list_free(&list);
    return out;
}

int SqliteRepository::addNote(const Note &note)
{
    if (m_db == nullptr) return 0;

    DictNote c = fromNote(note);
    int newId = 0;
    if (dict_db_add_note(m_db, &c, &newId) != DICT_OK)
        return 0;
    return newId;
}

bool SqliteRepository::updateNote(const Note &note)
{
    if (m_db == nullptr) return false;

    DictNote c = fromNote(note);
    return dict_db_update_note(m_db, &c) == DICT_OK;
}

bool SqliteRepository::deleteNote(int noteId)
{
    if (m_db == nullptr) return false;
    return dict_db_delete_note(m_db, noteId) == DICT_OK;
}

QHash<int, QVector<Note>> SqliteRepository::notesForFavorites(int limit) const
{
    QHash<int, QVector<Note>> byEntry;
    if (m_db == nullptr) return byEntry;

    if (limit <= 0) limit = -1;

    DictNoteList list = {};
    if (dict_db_list_favorite_notes(m_db, limit, &list) != DICT_OK)
        return byEntry;

    for (int i = 0; i < list.count; ++i) {
        const Note n = toNote(list.items[i]);
        byEntry[n.entryId].append(n);
    }

    dict_note_list_free(&list);
    return byEntry;
}
