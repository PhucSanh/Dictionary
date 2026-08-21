#ifndef DICT_DEINFLECT_H
#define DICT_DEINFLECT_H
#include "dict_types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define DICT_MAX_CANDIDATES 16

int dict_deinflect(const char *word,
                   char out[][DICT_WORD_LEN], int max);


#ifdef __cplusplus
}
#endif
#endif // DICT_DEINFLECT_H
