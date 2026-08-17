#ifndef DICT_KANA_H
#define DICT_KANA_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
    DICT_INPUT_UNKNOWN,
    DICT_INPUT_JAPANESE,
    DICT_INPUT_LATIN,
    DICT_INPUT_VIETNAMESE
} DictInputType;

int dict_kana_to_hiragana(const char *src, char *dst, size_t dst_size);
DictInputType dict_kana_detect(const char *s);

#ifdef __cplusplus
}
#endif

#endif // DICT_KANA_H
