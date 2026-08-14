#include "entrymodel.h"

EntryModel::EntryModel(QObject *parent)
    :QAbstractListModel(parent)
{
    Entry e;
    e.word = "食べる";
    e.reading = "たべる";
    e.meaning = "ăn";
    m_entries.append(e);

}

int EntryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_entries.size();

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
    return roles;

}
