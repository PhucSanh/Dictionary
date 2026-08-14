#pragma once
#include <QAbstractListModel>
#include <QVector>
#include <QtQml/qqmlregistration.h>
#include <qnamespace.h>
#include "entry.h"
class EntryModel : public QAbstractListModel{
    Q_OBJECT
    QML_ELEMENT
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

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
private:
  QVector<Entry> m_entries;
};

