
#include "dict_conjugate.h"
#include "dict_types.h"
#include "dict_utf8.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
static int ends_with(const char*s, const char* suffix){
    size_t ls = strlen(s),lf = strlen(suffix);
    if (lf > ls) return 0;
    return memcmp(s+ls-lf,suffix,lf) == 0;
}
static int is_e_row(uint32_t cp)
{
    static const uint32_t E_ROW[] = {
        0x3048, 0x3051, 0x305B, 0x3066, 0x306D,  /* え け せ て ね */
        0x3078, 0x3081, 0x308C, 0x3052, 0x305C,  /* へ め れ げ ぜ */
        0x3067, 0x3079, 0x307A, 0                /* で べ ぺ */
    };
    for (int i = 0; E_ROW[i]; i++)
        if (cp == E_ROW[i]) return 1;
    return 0;
}
static void add_form(DictForm out[], int *n, int max,
                     const char *name, const char *stem, const char *suffix)
{
    if (*n >= max) return;
    snprintf(out[*n].name, sizeof out[*n].name, "%s", name);
    snprintf(out[*n].text, sizeof out[*n].text, "%s%s", stem, suffix);
    (*n)++;
}
typedef struct {
    const char *word;
    const char *reading;
} VerbEntry;
static const VerbEntry I_ROW_ICHIDAN[] = {
    { "起きる", "おきる"   }, { "見る",   "みる"   },
    { "降りる", "おりる"   }, { "過ぎる", "すぎる" },
    { "足りる", "たりる"   }, { "似る",   "にる"   },
    { "生きる", "いきる"   }, { "閉じる", "とじる" },
    { "感じる", "かんじる" }, { "信じる", "しんじる" },
    { "応じる", "おうじる" }, { "禁じる", "きんじる" },
    { "演じる", "えんじる" }, { "存じる", "ぞんじる" },
    { "出来る", "できる"   }, { "浴びる", "あびる" },
    { "借りる", "かりる"   }, { "居る",   "いる"   },
    { "尽きる", "つきる"   }, { "着る",   "きる"   },
    { "落ちる", "おちる"   }, { "飽きる", "あきる" },
    { "煮る",   "にる"     },
    { NULL, NULL }
};
static const VerbEntry  E_ROW_GODAN[] = {
    { "帰る",   "かえる"   }, { "減る",   "へる"   },
    { "蹴る",   "ける"     }, { "喋る",   "しゃべる" },
    { "滑る",   "すべる"   }, { "湿る",   "しめる" },
    { NULL, NULL }
};
static int in_table(const VerbEntry *tbl,
                    const char *word, const char *reading)
{
    for (int i = 0; tbl[i].word != NULL; i++)
        if (strcmp(word, tbl[i].word) == 0 && strcmp(reading, tbl[i].reading) == 0)
            return 1;
    return 0;
}
DictVerbType dict_verb_type(const char *word, const char *reading_hira, const char *part_of_speech)
{
    if (part_of_speech == NULL || strstr(part_of_speech, "động từ") == NULL)
        return DICT_VERB_NONE;
    if (ends_with(word, "する") || ends_with(reading_hira, "する")) return DICT_VERB_SURU;
    if (ends_with(word, "来る") || ends_with(reading_hira, "くる"))
        return DICT_VERB_KURU;
    if(!ends_with(reading_hira,"る")) return DICT_VERB_GODAN;
    int off = dict_utf8_last_offset(reading_hira);
    if (off <= 0) return DICT_VERB_GODAN;
    char stem[DICT_READING_LEN];
    snprintf(stem, sizeof stem, "%.*s", off, reading_hira);

    int off2 = dict_utf8_last_offset(stem);
    uint32_t cp = 0;
    dict_utf8_decode(stem + off2, &cp);

    if (is_e_row(cp))
        return in_table(E_ROW_GODAN, word, reading_hira)
                   ? DICT_VERB_GODAN : DICT_VERB_ICHIDAN;

    return in_table(I_ROW_ICHIDAN, word, reading_hira)
               ? DICT_VERB_ICHIDAN : DICT_VERB_GODAN;


}
static int conjugate_ichidan(const char *word, DictForm out[], int max)
{
    int off = dict_utf8_last_offset(word);
    if (off <= 0) return 0;

    char stem[DICT_FORM_TEXT_LEN];
    snprintf(stem, sizeof stem, "%.*s", off, word);

    int n = 0;
    add_form(out, &n, max, "Từ điển",      word, "");
    add_form(out, &n, max, "ます",         stem, "ます");
    add_form(out, &n, max, "ました",       stem, "ました");
    add_form(out, &n, max, "ません",       stem, "ません");
    add_form(out, &n, max, "ませんでした", stem, "ませんでした");
    add_form(out, &n, max, "ない",         stem, "ない");
    add_form(out, &n, max, "なかった",     stem, "なかった");
    add_form(out, &n, max, "た (quá khứ)", stem, "た");
    add_form(out, &n, max, "て",           stem, "て");
    add_form(out, &n, max, "khả năng",     stem, "られる");
    add_form(out, &n, max, "bị động",      stem, "られる");
    add_form(out, &n, max, "sai khiến",    stem, "させる");
    add_form(out, &n, max, "mệnh lệnh",    stem, "ろ");
    add_form(out, &n, max, "điều kiện ば", stem, "れば");
    return n;
}
typedef struct {
    const char *plain;
    const char *i_row;
    const char *a_row;
    const char *e_row;
    const char *ta;
    const char *te;
} GodanRule;
static const GodanRule GODAN_RULES[] = {
    /* plain, i,     a,     e,     ta,   te */
    { "う",  "い",  "わ",  "え",  "った", "って" },
    { "つ",  "ち",  "た",  "て",  "った", "って" },
    { "る",  "り",  "ら",  "れ",  "った", "って" },
    { "ぬ",  "に",  "な",  "ね",  "んだ", "んで" },
    { "ぶ",  "び",  "ば",  "べ",  "んだ", "んで" },
    { "む",  "み",  "ま",  "め",  "んだ", "んで" },
    { "く",  "き",  "か",  "け",  "いた", "いて" },
    { "ぐ",  "ぎ",  "が",  "げ",  "いだ", "いで" },
    { "す",  "し",  "さ",  "せ",  "した", "して" },
    { NULL, NULL, NULL, NULL, NULL, NULL }
};
static int conjugate_godan(const char *word, DictForm out[], int max)
{
    int off = dict_utf8_last_offset(word);
    if (off <= 0) return 0;

    const char *tail = word + off;
    const GodanRule *r = NULL;
    for (int i = 0; GODAN_RULES[i].plain != NULL; i++) {
        if (strcmp(tail, GODAN_RULES[i].plain) == 0) {
            r = &GODAN_RULES[i];
            break;
        }
    }
    if (r == NULL) return 0;

    char stem[DICT_FORM_TEXT_LEN];
    snprintf(stem, sizeof stem, "%.*s", off, word);   /* 行く -> 行 */

    const char *ta = r->ta;
    const char *te = r->te;
    if (strcmp(word, "行く") == 0) { ta = "った"; te = "って"; }

    char buf[DICT_FORM_TEXT_LEN];
    int n = 0;

    add_form(out, &n, max, "Từ điển", word, "");

    snprintf(buf, sizeof buf, "%s%s", stem, r->i_row);
    add_form(out, &n, max, "ます",         buf, "ます");
    add_form(out, &n, max, "ました",       buf, "ました");
    add_form(out, &n, max, "ません",       buf, "ません");
    add_form(out, &n, max, "ませんでした", buf, "ませんでした");

    snprintf(buf, sizeof buf, "%s%s", stem, r->a_row);
    add_form(out, &n, max, "ない",     buf, "ない");
    add_form(out, &n, max, "なかった", buf, "なかった");
    add_form(out, &n, max, "bị động",  buf, "れる");
    add_form(out, &n, max, "sai khiến", buf, "せる");

    add_form(out, &n, max, "た (quá khứ)", stem, ta);
    add_form(out, &n, max, "て",           stem, te);

    snprintf(buf, sizeof buf, "%s%s", stem, r->e_row);
    add_form(out, &n, max, "khả năng",     buf, "る");
    add_form(out, &n, max, "mệnh lệnh",    buf, "");
    add_form(out, &n, max, "điều kiện ば", buf, "ば");

    return n;
}
static int conjugate_suru(const char *word, DictForm out[], int max)
{

    size_t len = strlen(word);
    char prefix[DICT_FORM_TEXT_LEN] = "";
    if (len >= 6)
        snprintf(prefix, sizeof prefix, "%.*s", (int)(len - 6), word);

    static const char *FORMS[][2] = {
        { "Từ điển",       "する"         },
        { "ます",          "します"       },
        { "ました",        "しました"     },
        { "ません",        "しません"     },
        { "ませんでした",  "しませんでした" },
        { "ない",          "しない"       },
        { "なかった",      "しなかった"   },
        { "た (quá khứ)",  "した"         },
        { "て",            "して"         },
        { "khả năng",      "できる"       },
        { "bị động",       "される"       },
        { "sai khiến",     "させる"       },
        { "mệnh lệnh",     "しろ"         },
        { "điều kiện ば",  "すれば"       },
        { NULL, NULL }
    };

    int n = 0;
    for (int i = 0; FORMS[i][0] != NULL; i++)
        add_form(out, &n, max, FORMS[i][0], prefix, FORMS[i][1]);
    return n;
}
static int conjugate_kuru(const char *word, DictForm out[], int max)
{

    size_t len = strlen(word);
    char prefix[DICT_FORM_TEXT_LEN] = "";
    if (len >= 6)
        snprintf(prefix, sizeof prefix, "%.*s", (int)(len - 6), word);

    static const char *FORMS[][2] = {
        { "Từ điển",       "来る (くる)"           },
        { "ます",          "来ます (きます)"       },
        { "ました",        "来ました (きました)"   },
        { "ません",        "来ません (きません)"   },
        { "ませんでした",  "来ませんでした"        },
        { "ない",          "来ない (こない)"       },
        { "なかった",      "来なかった (こなかった)" },
        { "た (quá khứ)",  "来た (きた)"           },
        { "て",            "来て (きて)"           },
        { "khả năng",      "来られる (こられる)"   },
        { "bị động",       "来られる (こられる)"   },
        { "sai khiến",     "来させる (こさせる)"   },
        { "mệnh lệnh",     "来い (こい)"           },
        { "điều kiện ば",  "来れば (くれば)"       },
        { NULL, NULL }
    };

    int n = 0;
    for (int i = 0; FORMS[i][0] != NULL; i++)
        add_form(out, &n, max, FORMS[i][0], prefix, FORMS[i][1]);
    return n;
}
int dict_conjugate(const char *word, const char *reading_hira,
                   const char *part_of_speech, DictForm out[], int max)
{
    if (word == NULL || out == NULL || max <= 0) return 0;

    switch (dict_verb_type(word, reading_hira, part_of_speech)) {
    case DICT_VERB_ICHIDAN: return conjugate_ichidan(word, out, max);
    case DICT_VERB_GODAN:   return conjugate_godan(word, out, max);
    case DICT_VERB_SURU:    return conjugate_suru(word, out, max);
    case DICT_VERB_KURU:    return conjugate_kuru(word, out, max);
    default:                return 0;
    }
}