#ifndef DICT_CONJUGATE_H
#define DICT_CONJUGATE_H


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DICT_VERB_NONE,
    DICT_VERB_ICHIDAN,
    DICT_VERB_GODAN,
    DICT_VERB_SURU,
    DICT_VERB_KURU
} DictVerbType;

DictVerbType dict_verb_type(const char *word, const char *reading_hira,
                            const char *part_of_speech);
#define DICT_FORM_NAME_LEN  32
#define DICT_FORM_TEXT_LEN  96
#define DICT_MAX_FORMS      14

typedef struct {
    char name[DICT_FORM_NAME_LEN];
    char text[DICT_FORM_TEXT_LEN];
} DictForm;
int dict_conjugate(const char *word, const char *reading_hira,
                   const char *part_of_speech,
                   DictForm out[], int max);
#ifdef __cplusplus
}
#endif

#endif // DICT_CONJUGATE_H
