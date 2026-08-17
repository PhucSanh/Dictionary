#pragma once
#include <QStringList>
#include <QString>
int columnIndex(const QStringList &header, const QString &name);
QString cell(const QStringList &row, int index);