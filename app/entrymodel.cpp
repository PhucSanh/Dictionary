#include "entrymodel.h"
#include "dict_db.h"
#include "dict_kana.h"
#include "dict_types.h"
#include <QGuiApplication>
#include <QClipboard>
#include <QCoreApplication>



EntryModel::EntryModel(QObject *parent)
    :QAbstractListModel(parent)
{
    const QString path = QCoreApplication::applicationDirPath() + "/dictionary.db";
    const QByteArray p = path.toUtf8();

    if (dict_db_open(p.constData(), &m_db) != DICT_OK)
        qWarning() << "Khong mo duoc db:" << path;

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

