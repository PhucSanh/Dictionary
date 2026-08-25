#include "ViewModel/EntryModel.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QStringList>
#include <QVariant>
#include <utility>

namespace {

constexpr int kPageSize       = 10;

QVariantMap toMap(const Note &n)
{
    QVariantMap m;
    m["noteId"]      = n.id;
    m["japanese"]    = n.japanese;
    m["translation"] = n.translation;
    m["note"]        = n.note;
    return m;
}

QVariantList toVariantList(const QVector<Note> &notes)
{
    QVariantList out;
    out.reserve(notes.size());
    for (const Note &n : notes)
        out.append(toMap(n));
    return out;
}

QVariantMap toMap(const Category &c)
{
    QVariantMap m;
    m["categoryId"] = c.id;
    m["name"]       = c.name;
    m["entryCount"] = c.entryCount;
    return m;
}

QVariantList toVariantList(const QVector<Category> &categories)
{
    QVariantList out;
    out.reserve(categories.size());
    for (const Category &c : categories)
        out.append(toMap(c));
    return out;
}

QVector<int> toIntVector(const QVariantList &ids)
{
    QVector<int> out;
    out.reserve(ids.size());
    for (const QVariant &v : ids) {
        const int id = v.toInt();
        if (id > 0 && !out.contains(id))
            out.append(id);
    }
    return out;
}

}

EntryModel::EntryModel(std::shared_ptr<IDictionaryRepository> dict,
                       std::shared_ptr<IUserRepository> user,
                       std::shared_ptr<ITextAnalyzer> text,
                       QObject *parent)
    : QAbstractListModel(parent)
    , m_dict(std::move(dict))
    , m_user(std::move(user))
    , m_text(std::move(text))
{
}

EntryModel::~EntryModel() = default;

int EntryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_entries.size();
}

QVariant EntryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size())
        return {};

    const Entry &e = m_entries.at(index.row());
    switch (role) {
    case IdRole:           return e.id;
    case WordRole:         return e.word;
    case ReadingRole:      return e.reading;
    case ReadingHiraRole:  return e.readingHira;
    case RomajiRole:       return e.romaji;
    case PartOfSpeechRole: return e.partOfSpeech;
    case MeaningRole:      return e.meaning;
    case EnglishRole:      return e.english;
    case LevelRole:        return e.level;
    case CategoriesRole:   return e.categories;
    default:               return {};
    }
}

QHash<int, QByteArray> EntryModel::roleNames() const
{
    return {
        { IdRole,           "entryId" },
        { WordRole,         "word" },
        { ReadingRole,      "reading" },
        { ReadingHiraRole,  "reading_hira" },
        { RomajiRole,       "romaji" },
        { PartOfSpeechRole, "part_of_speech" },
        { MeaningRole,      "meaning" },
        { EnglishRole,      "english" },
        { LevelRole,        "level" },
        { CategoriesRole,   "categories" },
    };
}

int     EntryModel::count() const           { return m_entries.size(); }
int     EntryModel::totalCount() const      { return m_totalCount; }
bool    EntryModel::hasMore() const         { return m_hasMore; }
int     EntryModel::mode() const            { return m_mode; }
int     EntryModel::categoryFilter() const  { return m_categoryFilter; }
QString EntryModel::deinflectedFrom() const { return m_deinflectedFrom; }
QString EntryModel::deinflectedTo() const   { return m_deinflectedTo; }

QVariantList EntryModel::categories() const
{
    if (!m_user)
        return {};
    return toVariantList(m_user->categories());
}

EntryModel::QueryOutcome EntryModel::runQuery(const QString &query,
                                              int limit, int offset) const
{
    if (!m_dict || !m_text)
        return {};

    switch (m_text->detect(query)) {
    case InputKind::Vietnamese:
        return { m_dict->searchByMeaning(query, limit, offset), true };

    case InputKind::Latin: {
        QVector<Entry> byWord = m_dict->searchByWord(query, limit, offset);
        if (!byWord.isEmpty())
            return { byWord, false };
        return { m_dict->searchByMeaning(query, limit, offset), true };
    }

    default:
        return { m_dict->searchByWord(query, limit, offset), false };
    }
}

void EntryModel::search(const QString &query)
{
    if (!m_dict)
        return;

    const QString q = query.trimmed();
    if (q.isEmpty()) {
        clear();
        return;
    }

    setMode(ModeSearch);
    m_lastQuery = q;
    m_offset    = 0;
    setDeinflected(QString(), QString());

    QueryOutcome outcome = runQuery(q, kPageSize, 0);
    QVector<Entry> entries = outcome.entries;
    m_usedMeaning = outcome.usedMeaning;
    int total = 0;

    if (!entries.isEmpty()) {
        total = m_usedMeaning ? m_dict->countByMeaning(q)
                              : m_dict->countByWord(q);
    } else if (m_text) {
        const QStringList candidates = m_text->deinflect(q);
        for (const QString &candidate : candidates) {
            QVector<Entry> found = m_dict->searchByWord(candidate, kPageSize, 0);
            if (found.isEmpty())
                continue;

            setDeinflected(q, candidate);
            m_lastQuery   = candidate;
            m_usedMeaning = false;
            total         = m_dict->countByWord(candidate);
            entries       = std::move(found);
            break;
        }
    }

    m_offset = entries.size();
    setHasMore(entries.size() == kPageSize);
    setTotalCount(total);
    setEntries(entries);
}

void EntryModel::loadMore()
{
    if (!m_hasMore || m_busy)
        return;

    m_busy = true;
    QVector<Entry> more;

    switch (m_mode) {
    case ModeSearch:
        if (m_dict && !m_lastQuery.isEmpty()) {
            more = m_usedMeaning
                 ? m_dict->searchByMeaning(m_lastQuery, kPageSize, m_offset)
                 : m_dict->searchByWord(m_lastQuery, kPageSize, m_offset);
        }
        break;

    case ModeHistory:
        if (m_user)
            more = m_user->recentHistory(kPageSize, m_offset);
        break;

    case ModeFavorites:
        if (m_user)
            more = m_user->favoritesInCategory(m_categoryFilter, kPageSize, m_offset);
        break;

    default:
        break;
    }

    m_offset += more.size();
    setHasMore(more.size() == kPageSize);
    appendEntries(more);
    m_busy = false;
}

void EntryModel::clear()
{
    m_lastQuery.clear();
    m_offset      = 0;
    m_usedMeaning = false;
    setHasMore(false);
    setTotalCount(0);
    setEntries({});
}

void EntryModel::showHistory()
{
    if (!m_user)
        return;

    const QVector<Entry> entries = m_user->recentHistory(kPageSize, 0);

    setMode(ModeHistory);
    m_lastQuery.clear();
    m_offset = entries.size();
    setHasMore(entries.size() == kPageSize);
    setTotalCount(m_user->historyCount());
    setDeinflected(QString(), QString());
    setEntries(entries);
}

void EntryModel::showFavorites(int categoryId)
{
    if (!m_user)
        return;

    setCategoryFilter(categoryId);
    const QVector<Entry> entries =
        m_user->favoritesInCategory(m_categoryFilter, kPageSize, 0);

    setMode(ModeFavorites);
    m_lastQuery.clear();
    m_offset = entries.size();
    setHasMore(entries.size() == kPageSize);
    setTotalCount(m_user->favoriteCount(m_categoryFilter));
    setDeinflected(QString(), QString());
    setEntries(entries);
}

void EntryModel::addHistory(int entryId)
{
    if (m_user)
        m_user->addHistory(entryId);
}

bool EntryModel::toggleFavorite(int entryId)
{
    return m_user ? m_user->toggleFavorite(entryId) : false;
}

bool EntryModel::isFavorite(int entryId)
{
    return m_user ? m_user->isFavorite(entryId) : false;
}

int EntryModel::addCategory(const QString &name)
{
    if (!m_user)
        return 0;

    const int newId = m_user->addCategory(name);
    if (newId > 0)
        emit categoriesChanged();
    return newId;
}

bool EntryModel::renameCategory(int categoryId, const QString &name)
{
    if (!m_user)
        return false;

    const bool ok = m_user->renameCategory(categoryId, name);
    if (ok)
        emit categoriesChanged();
    return ok;
}

bool EntryModel::deleteCategory(int categoryId)
{
    if (!m_user)
        return false;

    const bool ok = m_user->deleteCategory(categoryId);
    if (!ok)
        return false;

    emit categoriesChanged();
    if (m_categoryFilter == categoryId && m_mode == ModeFavorites)
        showFavorites(0);
    return true;
}

void EntryModel::refreshCategories()
{
    emit categoriesChanged();
}

bool EntryModel::addFavorite(int entryId, const QVariantList &categoryIds)
{
    if (!m_user)
        return false;

    const bool ok = m_user->addFavorite(entryId, toIntVector(categoryIds));
    if (ok)
        emit categoriesChanged();
    return ok;
}

bool EntryModel::removeFavorite(int entryId)
{
    if (!m_user)
        return false;

    const bool ok = m_user->removeFavorite(entryId);
    if (ok)
        emit categoriesChanged();
    return ok;
}

bool EntryModel::setFavoriteCategories(int entryId, const QVariantList &categoryIds)
{
    if (!m_user)
        return false;

    const bool ok = m_user->setFavoriteCategories(entryId, toIntVector(categoryIds));
    if (ok)
        emit categoriesChanged();
    return ok;
}

QVariantList EntryModel::categoriesFor(int entryId)
{
    if (!m_user)
        return {};
    return toVariantList(m_user->categoriesFor(entryId));
}

QVariantList EntryModel::categoryIdsFor(int entryId)
{
    if (!m_user)
        return {};

    QVariantList out;
    for (const Category &c : m_user->categoriesFor(entryId))
        out.append(c.id);
    return out;
}

int EntryModel::favoriteCount(int categoryId)
{
    return m_user ? m_user->favoriteCount(categoryId) : 0;
}

QString EntryModel::categoryNamesFor(int entryId)
{
    if (!m_user)
        return {};

    QStringList names;
    for (const Category &c : m_user->categoriesFor(entryId))
        names.append(c.name);
    return names.join(QStringLiteral(", "));
}

QVariantList EntryModel::notesFor(int entryId)
{
    if (!m_user)
        return {};
    return toVariantList(m_user->notesFor(entryId));
}

int EntryModel::addNote(int entryId, const QString &jp,
                        const QString &tr, const QString &note)
{
    if (!m_user)
        return 0;

    Note n;
    n.entryId     = entryId;
    n.japanese    = jp;
    n.translation = tr;
    n.note        = note;
    return m_user->addNote(n);
}

bool EntryModel::updateNote(int noteId, int entryId, const QString &jp,
                            const QString &tr, const QString &note)
{
    if (!m_user)
        return false;

    Note n;
    n.id          = noteId;
    n.entryId     = entryId;
    n.japanese    = jp;
    n.translation = tr;
    n.note        = note;
    return m_user->updateNote(n);
}

bool EntryModel::deleteNote(int noteId)
{
    return m_user ? m_user->deleteNote(noteId) : false;
}

QVariantList EntryModel::conjugationsFor(const QString &word,
                                         const QString &readingHira,
                                         const QString &partOfSpeech)
{
    if (!m_text)
        return {};

    QVariantList out;
    const QVector<ConjugationForm> forms =
        m_text->conjugate(word, readingHira, partOfSpeech);

    out.reserve(forms.size());
    for (const ConjugationForm &f : forms) {
        QVariantMap m;
        m["name"] = f.name;
        m["text"] = f.text;
        out.append(m);
    }
    return out;
}

QVariantList EntryModel::flashcards(int limit, int categoryId)
{
    if (!m_user)
        return {};

    const QVector<Entry>            favorites = m_user->favoritesInCategory(categoryId, limit, 0);
    const QHash<int, QVector<Note>> notes     =
        m_user->notesForFavoritesInCategory(categoryId, limit);

    QVariantList out;
    out.reserve(favorites.size());

    for (const Entry &e : favorites) {
        QVariantMap card;
        card["entryId"]      = e.id;
        card["word"]         = e.word;
        card["reading"]      = e.reading;
        card["readingHira"]  = e.readingHira;
        card["romaji"]       = e.romaji;
        card["partOfSpeech"] = e.partOfSpeech;
        card["meaning"]      = e.meaning;
        card["english"]      = e.english;
        card["level"]        = e.level;
        card["categories"]   = e.categories;
        card["notes"]        = toVariantList(notes.value(e.id));
        out.append(card);
    }
    return out;
}

void EntryModel::copyToClipboard(const QString &text)
{
    QGuiApplication::clipboard()->setText(text);
}

void EntryModel::setEntries(const QVector<Entry> &entries)
{
    const bool wasBusy = m_busy;
    m_busy = true;

    beginResetModel();
    m_entries = entries;
    endResetModel();

    m_busy = wasBusy;
    emit countChanged();
}

void EntryModel::appendEntries(const QVector<Entry> &entries)
{
    if (entries.isEmpty())
        return;

    const int first = m_entries.size();
    const int last  = first + entries.size() - 1;

    const bool wasBusy = m_busy;
    m_busy = true;

    beginInsertRows(QModelIndex(), first, last);
    m_entries.append(entries);
    endInsertRows();

    m_busy = wasBusy;
    emit countChanged();
}

void EntryModel::setHasMore(bool value)
{
    if (m_hasMore == value)
        return;
    m_hasMore = value;
    emit hasMoreChanged();
}

void EntryModel::setTotalCount(int total)
{
    if (m_totalCount == total)
        return;
    m_totalCount = total;
    emit totalCountChanged();
}

void EntryModel::setMode(int value)
{
    if (m_mode == value)
        return;
    m_mode = value;
    emit modeChanged();
}

void EntryModel::setCategoryFilter(int categoryId)
{
    const int value = categoryId > 0 ? categoryId : 0;
    if (m_categoryFilter == value)
        return;
    m_categoryFilter = value;
    emit categoryFilterChanged();
}

void EntryModel::setDeinflected(const QString &from, const QString &to)
{
    if (m_deinflectedFrom == from && m_deinflectedTo == to)
        return;
    m_deinflectedFrom = from;
    m_deinflectedTo   = to;
    emit deinflectedChanged();
}
