#include "csvreader.h"
#include <QFile>

QVector<QStringList> readCsvFile(const QString &path){
    QFile file (path);
    if(!file.open(QIODevice::ReadOnly)){
        return {};
    }
    QByteArray bytes = file.readAll();
    const QByteArray bom = QByteArray::fromHex("EFBBBF");

    if (bytes.startsWith(bom))
        bytes.remove(0,bom.size());

    return parseCsv(QString::fromUtf8(bytes));

}
QVector<QStringList> parseCsv(const QString &rawText){
    QString text = rawText;
    text.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));

    QVector<QStringList> rows;
    QStringList row;
    QString field;
    bool inQuotes = false;
    const int n = text.size();
    int i = 0;
    while (i < n) {
        const QChar c = text.at(i);

        if (inQuotes) {
            if (c == QLatin1Char('"')) {
                if (i + 1 < n && text.at(i + 1) == QLatin1Char('"')) {
                    field.append(QLatin1Char('"'));
                    i += 2;
                    continue;
                }
                inQuotes = false;
                ++i;
                continue;
            }
            field.append(c);
            ++i;
            continue;
        }

        if (c == QLatin1Char('"')) { inQuotes = true; ++i; continue; }

        if (c == QLatin1Char(',')) {
            row.append(field);
            field.clear();
            ++i;
            continue;
        }

        if (c == QLatin1Char('\n')) {
            row.append(field);
            field.clear();
            rows.append(row);
            row.clear();
            ++i;
            continue;
        }

        field.append(c);
        ++i;
    }

    if (!field.isEmpty() || !row.isEmpty()) {
        row.append(field);
        rows.append(row);
    }


    return rows;
}