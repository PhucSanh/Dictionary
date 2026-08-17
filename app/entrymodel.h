#pragma once
#include <QAbstractListModel>
#include <QVector>
#include <QtQml/qqmlregistration.h>
#include <qnamespace.h>
extern "C" {
#include "dict_db.h"
#include "entry.h"
}
class EntryModel : public QAbstractListModel{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
    Q_PROPERTY(bool hasMore READ hasMore  NOTIFY hasMoreChanged )
public:
    enum Roles{
        WordRole = Qt::UserRole+1,
        ReadingRole,
        RomajiRole,
        MeaningRole,
        PartOfSpeechRole,
        LevelRole
    };

    explicit EntryModel(QObject *parent = nullptr);
    ~EntryModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    int count() const;
    int totalCount() const;
    Q_INVOKABLE void search(const QString &query);
    Q_INVOKABLE void clear();
    bool hasMore() const;
    Q_INVOKABLE void loadMore();
signals:
    void countChanged();
    void hasMoreChanged();
    void totalCountChanged();
private:
  Entry toEntry(const DictEntry &c) const;
  void setHasMore(bool v);
  void setTotalCount(int total);
  void setFromList(const DictEntryList &list);
  void  appendFromList(const DictEntryList &list);
  DictDb *m_db = nullptr;
  QVector<Entry> m_entries;
  QString m_lastQuery;
  int     m_offset = 0;
  bool    m_hasMore = false;
  int m_totalCount = 0;
};



