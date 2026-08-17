#pragma once
#include <QStringList>
#include <QString>
QVector<QStringList> readCsvFile(const QString &path);
QVector<QStringList> parseCsv(const QString &rawText);
