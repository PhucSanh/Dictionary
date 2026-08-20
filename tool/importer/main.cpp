#include "csvreader.h"
#include "common.h"
#include "dict_conjugate.h"
#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>
#include <QElapsedTimer>
#include <QDebug>
#include <string.h>

extern "C" {
#include "dict_db.h"
#include "dict_kana.h"
#include "dict_types.h"
}

int main(int argc,char *argv[]){
   QCoreApplication app(argc, argv);
   QTextStream out(stdout);

   QStringList args = QCoreApplication::arguments();
   if (args.size() < 2) {
       out << "Cach dung: importer <input.csv> <output.db>" << Qt::endl;
       return 1;
   }
   const QString csvPath = args.at(1);
   const QString dbPath  = QCoreApplication::applicationDirPath() + "/dictionary.db";

   DictDb *db = NULL;
   if (dict_db_create(dbPath.toUtf8().constData(), &db) != DICT_OK) {
       out << "Khong tao duoc db" << Qt::endl;
       return 1;
   }
   const QVector<QStringList> data = readCsvFile(csvPath);
   int headRow = -1;
   for (int i = 0; i < qMin(10, data.size()); i++) {
       if (columnIndex(data.at(i), QStringLiteral("word")) >= 0) {
           headRow = i;
           break;
       }
   }
   if (headRow < 0) {
       out << "Khong tim thay dong tieu de co cot 'word'" << Qt::endl;
       dict_db_close(db);
       return 1;
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


    if (dict_db_begin(db) != DICT_OK) {
        out << "Khong bat dau duoc transaction: " << dict_db_last_error(db) << Qt::endl;
        return 1;
    }
    int inserted = 0, skipped = 0;
    for (int r = headRow+1; r < data.size(); ++r) {

        const QStringList &row = data.at(r);

        const QString word = cell(row, iWord);
        if (word.isEmpty()) { ++skipped; continue; }

        QString reading = cell(row, iReading);
        if (reading.isEmpty()) reading = word;
          const QString level = cell(row, iLevel);
        DictEntry e;
        memset(&e, 0, sizeof e);

        QByteArray bWord    = word.toUtf8();
        QByteArray bReading = reading.toUtf8();
        QByteArray bRomaji  = cell(row, iRomaji).toUtf8();
        QByteArray bPos     = cell(row, iPart_of_speech).toUtf8();
        QByteArray bMeaning = cell(row, iMeaningVi).toUtf8();
        QByteArray bEnglish = cell(row, iMeaningEn).toUtf8();
        QByteArray bLevel   = level.toUtf8();

        snprintf(e.word,    sizeof e.word,    "%s", bWord.constData());
        snprintf(e.reading, sizeof e.reading, "%s", bReading.constData());
        dict_kana_to_hiragana(e.reading, e.reading_hira, sizeof e.reading_hira);
        snprintf(e.romaji,         sizeof e.romaji,         "%s", bRomaji.constData());
        snprintf(e.part_of_speech, sizeof e.part_of_speech, "%s", bPos.constData());
        snprintf(e.meaning,        sizeof e.meaning,        "%s", bMeaning.constData());
        snprintf(e.english,        sizeof e.english,        "%s", bEnglish.constData());
        snprintf(e.level,          sizeof e.level,          "%s", bLevel.constData());

        if (dict_db_insert(db, &e) != DICT_OK) {
            out << "Insert loi: " << dict_db_last_error(db) << Qt::endl;
            dict_db_rollback(db);
            return 1;
        }
        inserted++;
    }
    if (dict_db_commit(db) != DICT_OK) {
        out << "COMMIT loi: " << dict_db_last_error(db) << Qt::endl;
        return 1;
    }

    if(dict_db_create_indexes(db) != DICT_OK){
       out << "CREATE INDEX loi: " << dict_db_last_error(db) << Qt::endl;
    }
    if (dict_db_create_fts(db) != DICT_OK)
        out << "FTS5 loi: " << dict_db_last_error(db) << Qt::endl;
    out << "inserted=" << inserted << " skipped=" << skipped << Qt::endl;
    out << "db: " << dbPath << Qt::endl;
    dict_db_close(db);



   return 0;
}