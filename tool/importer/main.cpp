#include "csvreader.h"
#include <QSqlDatabase>
#include <QCoreApplication>
#include <QTextStream>
#include <QSqlQuery>
#include <QStringList>
#include <Dictionary.h>
#include "common.h"
#include <QFile>
#include "dict_utf8.h"
#include "kana.h"
#include <QElapsedTimer>
#include <QSqlError>
#include <qdebug.h>
int main(int argc,char *argv[]){
   QCoreApplication app(argc, argv);
   QTextStream out(stdout);

   QStringList args = QCoreApplication::arguments();
   if (args.size() < 3) {
       out << "Cach dung: importer <input.csv> <output.db>" << Qt::endl;
       return 1;
   }
   const QString csvPath = args.at(1);
   const QString dbPath  = QCoreApplication::applicationDirPath() + "/dictionary.db";
   if(QFile::exists(dbPath)){
       QFile::remove(dbPath);
   }
   QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

   db.setDatabaseName(dbPath);
   if(!db.open()){
       out << "error when open db";
   }
   QElapsedTimer timer;
   timer.start();
   QSqlQuery q(db);
   const QString createSql = QStringLiteral(
       "CREATE TABLE words ("
       "  id           INTEGER PRIMARY KEY AUTOINCREMENT,"
       "  word         TEXT NOT NULL,"
       "  reading      TEXT NOT NULL,"
       "  reading_hira TEXT NOT NULL,"
       "  romaji       TEXT,"
       "  part_of_speech          TEXT,"
       "  meaning   TEXT,"
       "  english   TEXT,"
       "  level        TEXT"
       ")");
   if(!q.exec(createSql)){
       out << "error when create sql" << Qt::endl;
   }

   const QVector<QStringList> data = readCsvFile(csvPath);
   int headRow = -1;
   for(int i=0;i<qMin(10,data.size());i++){
       if(columnIndex(data.at(i), "word") >= 0){
           headRow=i;
           break;
       }
   }
   const QStringList header = data.at(headRow);
   out << "All column" << " " << header.join(QStringLiteral("|")) << Qt::endl;
   const int iWord      = columnIndex(header, QStringLiteral("word"));

   if(iWord<0) {
       out << "Something error";
       return 1;
   }
   const int iReading   = columnIndex(header, QStringLiteral("reading"));
   const int iRomaji    = columnIndex(header, QStringLiteral("romaji"));
   const int iPart_of_speech    = columnIndex(header, QStringLiteral("part_of_speech"));
   const int iMeaningVi = columnIndex(header, QStringLiteral("meaning"));
   const int iMeaningEn = columnIndex(header, QStringLiteral("english"));
   const int iLevel     = columnIndex(header, QStringLiteral("level"));
    QSqlQuery insert(db);
   insert.prepare(QStringLiteral(
       "INSERT INTO words (word, reading, reading_hira, romaji, part_of_speech, meaning, english, level) "
       "VALUES (:word, :reading, :reading_hira, :romaji, :part_of_speech, :meaning, :english, :level)"));
    if (!db.transaction()) {
        out << "Khong bat dau duoc transaction: " << db.lastError().text() << Qt::endl;
        return 1;
    }
    int inserted = 0, skipped = 0;
    for (int r = headRow+1; r < data.size(); ++r) {
        const QStringList row = data.at(r);
        const QString word = cell(row, iWord);
        if (word.isEmpty()) { ++skipped; continue; }
        QString reading = cell(row,iReading);
        if(reading.isEmpty()) reading = word;
        QString level = cell(row, iLevel);
        insert.bindValue(QStringLiteral(":word"),         word);
        insert.bindValue(QStringLiteral(":reading"),      reading);
        insert.bindValue(QStringLiteral(":reading_hira"), katakanaToHiragana(reading));
        insert.bindValue(QStringLiteral(":romaji"),       cell(row, iRomaji));
        insert.bindValue(QStringLiteral(":part_of_speech"),          cell(row, iPart_of_speech));
        insert.bindValue(QStringLiteral(":meaning"),   cell(row, iMeaningVi));
        insert.bindValue(QStringLiteral(":english"),   cell(row, iMeaningEn));
        insert.bindValue(QStringLiteral(":level"),level.isEmpty() ? QVariant(QMetaType(QMetaType::QString)) : QVariant(level));
        if (!insert.exec()){
            out << "Error: " << insert.lastError().text() << Qt::endl;
            db.rollback();
            return 1;
        }
        inserted++;
    }
    if (!db.commit()) {
        out << "COMMIT loi: " << db.lastError().text() << Qt::endl;
        return 1;
    }

    q.exec(QStringLiteral("CREATE INDEX idx_words_word   ON words(word         COLLATE NOCASE)"));
    q.exec(QStringLiteral("CREATE INDEX idx_words_hira   ON words(reading_hira COLLATE NOCASE)"));
    q.exec(QStringLiteral("CREATE INDEX idx_words_romaji ON words(romaji       COLLATE NOCASE)"));
    q.exec(QStringLiteral(
        "CREATE VIRTUAL TABLE words_fts USING fts5(meaning, english)"));
    q.exec("INSERT INTO words_fts(rowid, meaning, english) "
           "SELECT id, meaning, english FROM words");
    out << skipped << " " << inserted << " " << timer.elapsed() << Qt::endl;
    Dictionary dict(dbPath);
    qDebug() << "isOpen:" << dict.isOpen() << dict.lastError();

    for (const Entry &e : dict.search("あい", 10))
        qDebug() << e.word << e.reading << e.meaning;

   return 0;
}