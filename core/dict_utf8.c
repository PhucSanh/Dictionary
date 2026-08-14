#include <stddef.h>
#include "dict_utf8.h"
int dict_utf8_decode(const char *s, uint32_t *cp)
{
    if (s == NULL || cp == NULL) return 0;

    unsigned char b0 = (unsigned char)s[0];
    if (b0 == 0) return 0;
    if (b0 < 0x80) {
        *cp = b0;
        return 1;
    }
    if(b0>=0xC0 && b0<=0xDF){
        unsigned char b1 = (unsigned char)s[1];
        *cp = ((uint32_t)(b0 & 0x1F) << 6)
              |  (uint32_t)(b1 & 0x3F);
        return 2;
    }
    if (b0 >= 0xE0 && b0 < 0xF0) {
        unsigned char b1 = (unsigned char)s[1];
        unsigned char b2 = (unsigned char)s[2];

        if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80) return 0;

        *cp = ((uint32_t)(b0 & 0x0F) << 12)
              | ((uint32_t)(b1 & 0x3F) <<  6)
              |  (uint32_t)(b2 & 0x3F);
        return 3;
    }
    if (b0 >= 0xF0 && b0 <= 0xF7){
        unsigned char b1 = (unsigned char)s[1];
        unsigned char b2 = (unsigned char)s[2];
        unsigned char b3 = (unsigned char)s[3];
        *cp = ((uint32_t)(b0 & 0x07) << 18)
              | ((uint32_t)(b1 & 0x3F) <<  12)
              |  ((uint32_t)(b2 & 0x3F)<<6)
              |((uint32_t)(b3 & 0x3F));
        return 4;

    }
    return 0;
}

int dict_utf8_encode(uint32_t cp, char *out)
{
    if (out == NULL) return 0;

    if (cp < 0x80) {
        out[0] = (char)cp;
        return 1;
    }
    if (cp < 0x800) {
        out[0] = (char)(0xC0 | (cp >> 6));
        out[1] = (char)(0x80 | (cp & 0x3F));
        return 2;
    }
    if (cp < 0x10000) {
        out[0] = (char)(0xE0 | (cp >> 12));
        out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
        out[2] = (char)(0x80 | (cp & 0x3F));
        return 3;
    }
    if (cp < 0x110000) {
        out[0] = (char)(0xF0 | (cp >> 18));
        out[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
        out[2] = (char)(0x80 | ((cp >> 6) & 0x3F));
        out[3] = (char)(0x80 | (cp & 0x3F));
        return 4;
    }
    return 0;

}
