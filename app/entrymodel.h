#pragma once

#include "domain/Entry.h"
#include "domain/IDictionaryRepository.h"
#include "domain/ITextAnalyzer.h"
#include "domain/IUserRepository.h"

#include <QAbstractListModel>
#include <QVector>
#include <QtQml/qqmlregistration.h>

#include <memory>

class EntryModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("EntryModel duoc tao o main.cpp va tiem vao qua Main.qml")

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
    Q_PROPERTY(bool hasMore READ hasMore NOTIFY hasMoreChanged)
    Q_PROPERTY(int mode READ mode NOTIFY modeChanged)
    Q_PROPERTY(QString deinflectedFrom READ deinflectedFrom NOTIFY deinflectedChanged)
    Q_PROPERTY(QString deinflectedTo READ deinflectedTo NOTIFY deinflectedChanged)
    Q_PROPERTY(QVariantList categories READ categories NOTIFY categoriesChanged)
    Q_PROPERTY(int categoryFilter READ categoryFilter NOTIFY categoryFilterChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        WordRole,
        ReadingRole,
        RomajiRole,
        MeaningRole,
        EnglishRole,
        PartOfSpeechRole,
        LevelRole,
        ReadingHiraRole,
        CategoriesRole
    };

    enum Mode { ModeSearch, ModeHistory, ModeFavorites };
    Q_ENUM(Mode)

    EntryModel(std::shared_ptr<IDictionaryRepository> dict,
               std::shared_ptr<IUserRepository> user,
               std::shared_ptr<ITextAnalyzer> text,
               QObject *parent = nullptr);

    ~EntryModel() override;

    int      rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int     count() const;
    int     totalCount() const;
    bool    hasMore() const;
    int     mode() const;
    QString deinflectedFrom() const;
    QString deinflectedTo() const;

    QVariantList categories() const;
    int          categoryFilter() const;

    Q_INVOKABLE void search(const QString &query);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void loadMore();
    Q_INVOKABLE void showHistory();
    Q_INVOKABLE void showFavorites(int categoryId = 0);

    Q_INVOKABLE void addHistory(int entryId);
    Q_INVOKABLE bool toggleFavorite(int entryId);
    Q_INVOKABLE bool isFavorite(int entryId);

    Q_INVOKABLE int  addCategory(const QString &name);
    Q_INVOKABLE bool renameCategory(int categoryId, const QString &name);
    Q_INVOKABLE bool deleteCategory(int categoryId);
    Q_INVOKABLE void refreshCategories();

    Q_INVOKABLE bool         addFavorite(int entryId, const QVariantList &categoryIds);
    Q_INVOKABLE bool         removeFavorite(int entryId);
    Q_INVOKABLE bool         setFavoriteCategories(int entryId, const QVariantList &categoryIds);
    Q_INVOKABLE QVariantList categoriesFor(int entryId);
    Q_INVOKABLE QVariantList categoryIdsFor(int entryId);
    Q_INVOKABLE QString      categoryNamesFor(int entryId);
    Q_INVOKABLE int          favoriteCount(int categoryId = 0);

    Q_INVOKABLE QVariantList notesFor(int entryId);
    Q_INVOKABLE int          addNote(int entryId, const QString &jp,
                                     const QString &tr, const QString &note);
    Q_INVOKABLE bool         updateNote(int noteId, int entryId, const QString &jp,
                                        const QString &tr, const QString &note);
    Q_INVOKABLE bool         deleteNote(int noteId);

    Q_INVOKABLE QVariantList conjugationsFor(const QString &word,
                                             const QString &readingHira,
                                             const QString &partOfSpeech);
    Q_INVOKABLE QVariantList flashcards(int limit, int categoryId = 0);

    Q_INVOKABLE void copyToClipboard(const QString &text);

signals:
    void countChanged();
    void totalCountChanged();
    void hasMoreChanged();
    void modeChanged();
    void deinflectedChanged();
    void categoriesChanged();
    void categoryFilterChanged();

private:
    struct QueryOutcome {
        QVector<Entry> entries;
        bool           usedMeaning = false;
    };
    QueryOutcome runQuery(const QString &query, int limit, int offset) const;

    void setEntries(const QVector<Entry> &entries);
    void appendEntries(const QVector<Entry> &entries);
    void setHasMore(bool value);
    void setTotalCount(int total);
    void setMode(int value);
    void setDeinflected(const QString &from, const QString &to);
    void setCategoryFilter(int categoryId);

    std::shared_ptr<IDictionaryRepository> m_dict;
    std::shared_ptr<IUserRepository>   m_user;
    std::shared_ptr<ITextAnalyzer>         m_text;

    QVector<Entry> m_entries;
    QString        m_lastQuery;
    int            m_offset     = 0;
    bool           m_usedMeaning = false;
    bool           m_hasMore    = false;
    bool           m_busy       = false;
    int            m_totalCount = 0;
    int            m_mode       = ModeHistory;
    int            m_categoryFilter = 0;
    QString        m_deinflectedFrom;
    QString        m_deinflectedTo;
};
