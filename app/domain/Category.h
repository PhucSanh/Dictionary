#pragma once
#include <QString>

struct Category {
    int     id         = 0;
    QString name;
    int     entryCount = 0;

    bool isValid() const { return id > 0; }
};
