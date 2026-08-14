#include "dictionary.h"
#include "kana.h"
#include <qsqldatabase.h>
#include <QSqlError>
#include <QSqlQuery>
Dictionary::Dictionary(const QString &dbPath)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE","dictionary_db");
    m_db.setDatabaseName(dbPath);
    if(!m_db.open()){
        m_error = m_db.lastError().text();
    }

}

Dictionary::~Dictionary()
{
    m_db.close();

}

bool Dictionary::isOpen() const
{
    return m_db.isOpen();

}

QString Dictionary::lastError() const
{
    return m_error;
}

QVector<Entry> Dictionary::search(const QString &query, int limit) const
{
    const QString hiragana = katakanaToHiragana(query);
    QVector<Entry> results{};
    QSqlQuery q(m_db);
    q.prepare(
        "SELECT id, word, reading, reading_hira, romaji, "
        "       part_of_speech, meaning, english, level "
        "FROM words "
        "WHERE word LIKE :raw OR reading_hira LIKE :keyword OR romaji LIKE :raw "
        "LIMIT :lim");
    q.bindValue(":raw",query+"%");
    q.bindValue(":keyword",hiragana+"%");
    q.bindValue(":lim", limit);
    if (!q.exec()) {
        qWarning() << "search loi:" << q.lastError().text();
        return results;
    }
    while(q.next()){
        Entry e;
        e.id = q.value("id").toInt();
        e.word         = q.value("word").toString();
        e.reading      = q.value("reading").toString();
        e.readingHira  = q.value("reading_hira").toString();
        e.romaji = q.value("romaji").toString();
        e.partOfSpeech = q.value("part_of_speech").toString();
        e.meaning = q.value("meaning").toString();
        e.english = q.value("english").toString();
        e.level = q.value("level").toString();
        results.append(e);

    }
    return results;

}

QVector<Entry> Dictionary::searchByMeaning(const QString &query, int limit) const
{
    QVector<Entry> results;
    QSqlQuery q(m_db);
    q.prepare(
        "SELECT w.id, w.word, w.reading, w.reading_hira, w.romaji, "
        "       w.part_of_speech, w.meaning, w.english, w.level "
        "FROM words_fts f "
        "JOIN words w ON w.id = f.rowid "
        "WHERE f.words_fts MATCH :q "
        "LIMIT :lim");
    q.bindValue(":q", query);
    q.bindValue(":lim", limit);

    if (!q.exec()) {
        qWarning() << "searchByMeaning loi:" << q.lastError().text();
        return results;
    }

    while (q.next()) {
        Entry e;
        e.id           = q.value("id").toInt();
        e.word         = q.value("word").toString();
        e.reading      = q.value("reading").toString();
        e.readingHira  = q.value("reading_hira").toString();
        e.romaji       = q.value("romaji").toString();
        e.partOfSpeech = q.value("part_of_speech").toString();
        e.meaning      = q.value("meaning").toString();
        e.english      = q.value("english").toString();
        e.level        = q.value("level").toString();
        results.append(e);
    }
    return results;

}
