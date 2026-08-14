#include "kana.h"

QString katakanaToHiragana(const QString &input){
    QString out{};
    for (const QChar ch : input) {
        const ushort u = ch.unicode();
        if (u >= 0x30A1 && u <= 0x30F6) {
            out.append(QChar(static_cast<ushort>(u - 0x60)));
            continue;
        }
        if (u == 0x30FD || u == 0x30FE) {
            out.append(QChar(static_cast<ushort>(u - 0x60)));
            continue;
        }
        out.append(ch);

    }
    return out;

}
