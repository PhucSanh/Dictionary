#ifndef DICT_UTF8_H
#define DICT_UTF8_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

    int dict_utf8_decode(const char *s, uint32_t *cp);
    int dict_utf8_encode(uint32_t cp, char *out);
    int dict_utf8_last_offset(const char *s);

#ifdef __cplusplus
}
#endif

#endif