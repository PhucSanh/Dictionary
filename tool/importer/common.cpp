#include "common.h"
int columnIndex(const QStringList &header, const QString &name){
    for (int i=0;i<header.size();i++) {
        if(header.at(i).trimmed().compare(name,Qt::CaseInsensitive)==0){
            return i;
        }
    }
    return -1;
}
QString cell(const QStringList &row, int index){
    if(index<0 || index>=row.size()) return {};
    return row.at(index);
}