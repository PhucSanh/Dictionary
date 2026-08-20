#include "entrymodel.h"
#include "dict_conjugate.h"
#include "dict_db.h"
#include "dict_kana.h"
#include "dict_types.h"
#include <QGuiApplication>
#include <QClipboard>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <cstdio>
#include <qcontainerfwd.h>

EntryModel::EntryModel(QObject *parent)
    :QAbstractListModel(parent)
{
    const QString path = QCoreApplication::applicationDirPath() + "/dictionary.db";
    const QByteArray p = path.toUtf8();

    int rc = dict_db_open(p.constData(), &m_db);
    if (rc != DICT_OK) return;
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    const QString userPath = dir + "/user.db";
    const QByteArray up = userPath.toUtf8();
   // qDebug() << "user.db =" << userPath;

    if (dict_db_attach_user(m_db, up.constData()) != DICT_OK)
        qWarning() << "Attach user.db loi:" << dict_db_last_error(m_db);

}

EntryModel::~EntryModel()
{
   dict_db_close(m_db);
}

int EntryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_entries.size();

}
static const int kPageSize = 10;

static int runQuery(DictDb *db, const QByteArray &q, int limit, int offset,
                    DictEntryList *out)
{
    switch (dict_kana_detect(q.constData())) {
    case DICT_INPUT_VIETNAMESE:
        return dict_db_search_meaning(db, q.constData(), limit, offset, out);

    case DICT_INPUT_LATIN: {
        int rc = dict_db_search(db, q.constData(), limit, offset, out);
        if (rc == DICT_OK && out->count == 0) {
            dict_entry_list_free(out);
            rc = dict_db_search_meaning(db, q.constData(), limit, offset, out);
        }
        return rc;
    }
    default:
        return dict_db_search(db, q.constData(), limit, offset, out);
    }
}

QVariant EntryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size()) return {};
    const Entry &e = m_entries.at(index.row());
    switch (role) {
    case IdRole:
        return e.id;
        break;
    case WordRole:
        return e.word;
        break;
    case ReadingRole:
        return e.reading;
        break;
    case RomajiRole:
        return e.romaji;
        break;
    case MeaningRole:
        return e.meaning;
        break;
    case PartOfSpeechRole:
        return e.partOfSpeech;
        break;
    case LevelRole:
        return e.level;
        break;
    case EnglishRole:
        return e.english;
        break;
    case ReadingHiraRole:
        return e.readingHira;
    default:
        break;
    }

    return {};

}

QHash<int, QByteArray> EntryModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[WordRole]    = "word";
    roles[ReadingRole] = "reading";
    roles[RomajiRole]  = "romaji";
    roles[MeaningRole] = "meaning";
    roles[PartOfSpeechRole]     = "part_of_speech";
    roles[LevelRole]   = "level";
    roles[EnglishRole] = "english";
    roles[IdRole] = "entryId";
    roles[ReadingHiraRole] = "reading_hira";
    return roles;

}

int EntryModel::count() const
{
    return m_entries.count();
}

int EntryModel::totalCount() const
{
    return m_totalCount;

}

void EntryModel::search(const QString &query)
{
    if (m_db == nullptr) return;


    const QString q = query.trimmed();
    if (q.isEmpty()) { clear(); return; }
    if(m_lastQuery!=q){
        int total= 0;
        int rc = dict_db_count_total(m_db,q.toUtf8().constData(),&total);
        if(rc == DICT_OK){
            setTotalCount(total);
        }
    }
    m_lastQuery = q;
    m_offset = 0;

    DictEntryList list = {0};
    const QByteArray qb = q.toUtf8();

    if (runQuery(m_db, qb, kPageSize, 0, &list) == DICT_OK) {
        setFromList(list);
        m_offset = list.count;
        setHasMore(list.count == kPageSize);
        setMode(ModeSearch);
        dict_entry_list_free(&list);
    }
    else {
         clear();
    }

}



void EntryModel::clear()
{
    beginResetModel();
    m_entries.clear();
    endResetModel();
    m_lastQuery.clear();
    m_offset = 0;
    m_totalCount = 0;
    setHasMore(false);
    emit totalCountChanged();
    emit countChanged();

}

void EntryModel::copyToClipboard(const QString &text)
{
    QGuiApplication::clipboard()->setText(text);

}

bool EntryModel::hasMore() const
{
    return m_hasMore;

}

void EntryModel::loadMore()
{
      if (m_db == nullptr || !m_hasMore || m_lastQuery.isEmpty()) return;
      DictEntryList list = {0};
      const QByteArray qb = m_lastQuery.toUtf8();

      if (runQuery(m_db, qb, kPageSize, m_offset, &list) == DICT_OK) {
          appendFromList(list);
          m_offset += list.count;
          setHasMore(list.count == kPageSize);
          dict_entry_list_free(&list);
      }

}

void EntryModel::addHistory(int entryId)
{
    if(m_db == nullptr) return;
    dict_db_add_history(m_db,entryId);

}

bool EntryModel::toggleFavorite(int entryId)
{
    if(m_db == nullptr) return false;
    int fav = 0;
    dict_db_toggle_favorite(m_db, entryId, &fav);
    return fav != 0;


}

bool EntryModel::isFavorite(int entryId)
{
    if (m_db == nullptr) return false;
    int fav = 0;
    dict_db_is_favorite(m_db, entryId, &fav);
    return fav != 0;

}

void EntryModel::showHistory()
{
    if (m_db == nullptr) return;

    DictEntryList list = {0};
    if (dict_db_list_history(m_db, 50, &list) == DICT_OK) {
        setFromList(list);
        setMode(ModeHistory);
        dict_entry_list_free(&list);
    }
    m_lastQuery.clear();
    m_offset = 0;
    setHasMore(false);
    setTotalCount(m_entries.size());

}

void EntryModel::showFavorites()
{
    if (m_db == nullptr) return;

    DictEntryList list = {0};
    if (dict_db_list_favorites(m_db, 50, &list) == DICT_OK) {
        setFromList(list);
        setMode(ModeFavorites);
        dict_entry_list_free(&list);
    }
    m_lastQuery.clear();
    m_offset = 0;
    setHasMore(false);
    setTotalCount(m_entries.size());

}

int EntryModel::mode() const
{
    return m_mode;

}

QVariantList EntryModel::notesFor(int entryId)
{
    QVariantList result;
    if (m_db == nullptr) return result;
    DictNoteList lists ={0};
    if(dict_db_list_notes(m_db,entryId,&lists) == DICT_OK){
        for(int i=0;i<lists.count;i++){
            const DictNote &value = lists.items[i];
            QVariantMap m;
            m["noteId"] = value.id;
            m["japanese"] = QString::fromUtf8(value.japanese);
            m["translation"] = QString::fromUtf8(value.translation);
            m["note"]        = QString::fromUtf8(value.note);
            result.append(m);
        }
        dict_note_list_free(&lists);
    }
    return result;

}

int EntryModel::addNote(int entryId, const QString &jp, const QString &tr, const QString &note)
{
    if (m_db == nullptr) return DICT_ERR_ARG;
    DictNote value;
    memset(&value,0,sizeof(value));
    value.entry_id = entryId;
    const QByteArray bjp = jp.toUtf8();
    const QByteArray btr = tr.toUtf8();
    const QByteArray bnt = note.toUtf8();
    snprintf(value.japanese,sizeof value.japanese,"%s",bjp.constData());
    snprintf(value.translation,sizeof value.translation,"%s",btr.constData());
    snprintf(value.note,sizeof value.note,"%s",bnt.constData());
    int id = 0;
    if(dict_db_add_note(m_db,&value,&id)!=DICT_OK) return 0;
    return id;


}

bool EntryModel::updateNote(int noteId, int entryId, const QString &jp, const QString &tr, const QString &note)
{
    if (m_db == nullptr) return DICT_ERR_ARG;
    DictNote value;
    memset(&value,0,sizeof(value));
    value.entry_id = entryId;
    value.id = noteId;
    const QByteArray bjp = jp.toUtf8();
    const QByteArray btr = tr.toUtf8();
    const QByteArray bnt = note.toUtf8();
    snprintf(value.japanese,sizeof value.japanese,"%s",bjp.constData());
    snprintf(value.translation,sizeof value.translation,"%s",btr.constData());
    snprintf(value.note,sizeof value.note,"%s",bnt.constData());
    if(dict_db_update_note(m_db,&value) != DICT_OK) return false;
    return true;
}

bool EntryModel::deleteNote(int noteId)
{
    if (m_db == nullptr) return DICT_ERR_ARG;
    if(dict_db_delete_note(m_db,noteId)!=DICT_OK) return false;
    return true;

}

QVariantList EntryModel::conjugationsFor(const QString &word, const QString &readingHira, const QString &partOfSpeech)
{
    QVariantList result;

    const QByteArray bw = word.toUtf8();
    const QByteArray br = readingHira.toUtf8();
    const QByteArray bp = partOfSpeech.toUtf8();

    DictForm forms[DICT_MAX_FORMS];
    int n = dict_conjugate(bw.constData(), br.constData(), bp.constData(),
                           forms, DICT_MAX_FORMS);

    for (int i = 0; i < n; ++i) {
        QVariantMap m;
        m["name"] = QString::fromUtf8(forms[i].name);
        m["text"] = QString::fromUtf8(forms[i].text);
        result.append(m);
    }
    return result;

}
void EntryModel::setMode(int m)
{
    if (m_mode == m) return;
    m_mode = m;
    emit modeChanged();
}

Entry EntryModel::toEntry(const DictEntry &c) const
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

void EntryModel::setHasMore(bool v)
{
    if(m_hasMore == v) return;
    m_hasMore = v;
    emit hasMoreChanged();

}

void EntryModel::setTotalCount(int total)
{
    m_totalCount = total;
    emit totalCountChanged();

}

void EntryModel::setFromList(const DictEntryList &list)
{
    beginResetModel();
    m_entries.clear();
    m_entries.reserve(list.count);

    for (int i = 0; i < list.count; ++i) {
        m_entries.append(toEntry(list.items[i]));
    }

    endResetModel();
    emit countChanged();

}

void EntryModel::appendFromList(const DictEntryList &list)
{
    if(list.count == 0) return;
    const int first = m_entries.size();
    const int last = first+list.count-1;
    beginInsertRows(QModelIndex(), first, last);
    for (int i = 0; i < list.count; ++i)
        m_entries.append(toEntry(list.items[i]));
    endInsertRows();
    emit countChanged();

}

