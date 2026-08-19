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
    Q_PROPERTY(int mode READ mode NOTIFY modeChanged)
public:
    enum Roles{
        IdRole = Qt::UserRole + 1,
        WordRole,
        ReadingRole,
        RomajiRole,
        MeaningRole,
        EnglishRole,
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
    Q_INVOKABLE void copyToClipboard(const QString &text);
    bool hasMore() const;
    Q_INVOKABLE void loadMore();
    Q_INVOKABLE void addHistory(int entryId);
    Q_INVOKABLE bool toggleFavorite(int entryId);
    Q_INVOKABLE bool isFavorite(int entryId);
    Q_INVOKABLE void showHistory();
    Q_INVOKABLE void showFavorites();
    enum Mode { ModeSearch, ModeHistory, ModeFavorites };
    Q_ENUM(Mode)
    int mode() const;
    Q_INVOKABLE QVariantList notesFor(int entryId);
    Q_INVOKABLE int addNote(int entryId, const QString &jp,
                            const QString &tr, const QString &note);
    Q_INVOKABLE bool updateNote(int noteId, int entryId, const QString &jp,
                                const QString &tr, const QString &note);
    Q_INVOKABLE bool deleteNote(int noteId);
signals:
    void countChanged();
    void hasMoreChanged();
    void totalCountChanged();
     void modeChanged();
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
   void setMode(int m);
  int m_mode = ModeHistory;
};



