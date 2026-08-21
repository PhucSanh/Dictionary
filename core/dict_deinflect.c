#include "dict_deinflect.h"
#include <string.h>
#include <stdio.h>

static void try_add(char out[][DICT_WORD_LEN], int *n, int max,
                    const char *word, size_t lw,
                    const char *from, const char *to)
{
    if (*n >= max) return;

    size_t lf = strlen(from);
    if (lf == 0 || lf >= lw) return;
    if (memcmp(word + lw - lf, from, lf) != 0) return;

    char base[DICT_WORD_LEN];
    int need = snprintf(base, sizeof base, "%.*s%s", (int)(lw - lf), word, to);
    if (need < 0 || need >= (int)sizeof base) return;

    for (int i = 0; i < *n; i++)
        if (strcmp(out[i], base) == 0) return;

    snprintf(out[*n], DICT_WORD_LEN, "%s", base);
    (*n)++;
}

static const struct { const char *from, *to; } IRREGULAR[] = {
    { "しませんでした","する" }, { "しました","する" }, { "しません","する" },
    { "します","する" }, { "しなかった","する" }, { "しない","する" },
    { "した","する" },   { "して","する" },       { "できる","する" },
    { "される","する" }, { "しろ","する" },       { "すれば","する" },

    { "来ませんでした","来る" }, { "来ました","来る" }, { "来ません","来る" },
    { "来ます","来る" }, { "来なかった","来る" }, { "来ない","来る" },
    { "来た","来る" },   { "来て","来る" },       { "来られる","来る" },
    { "来させる","来る" },{ "来い","来る" },      { "来れば","来る" },

    { "きました","くる" }, { "きます","くる" }, { "こない","くる" },
    { "きた","くる" },     { "きて","くる" },

    { "行った","行く" }, { "行って","行く" },
    { "いった","いく" }, { "いって","いく" },
    { NULL, NULL }
};

static const char *ICHIDAN_TAILS[] = {
    "ませんでした", "ました", "ません", "ます",
    "なかった", "ない", "た", "て",
    "られる",
    "させる", "ろ", "れば",
    NULL
};

enum { R_I, R_A, R_E, R_TA, R_TE };

typedef struct { const char *dict, *i, *a, *e, *ta, *te; } GodanRow;

static const GodanRow GODAN[] = {
    { "う","い","わ","え","った","って" },
    { "つ","ち","た","て","った","って" },
    { "る","り","ら","れ","った","って" },
    { "ぬ","に","な","ね","んだ","んで" },
    { "ぶ","び","ば","べ","んだ","んで" },
    { "む","み","ま","め","んだ","んで" },
    { "く","き","か","け","いた","いて" },
    { "ぐ","ぎ","が","げ","いだ","いで" },
    { "す","し","さ","せ","した","して" },
    { NULL,NULL,NULL,NULL,NULL,NULL }
};

static const struct { int row; const char *tail; } GFORM[] = {
    { R_I,  "ませんでした" },
    { R_I,  "ました"       },
    { R_I,  "ません"       },
    { R_I,  "ます"         },
    { R_A,  "なかった"     },
    { R_A,  "ない"         },
    { R_TA, ""             },
    { R_TE, ""             },
    { R_A,  "れる"         },
    { R_A,  "せる"         },
    { R_E,  "る"           },
    { R_E,  "ば"           },
    { R_E,  ""             },
    { -1,   NULL }
};

static const char *row_kana(const GodanRow *g, int row)
{
    switch (row) {
    case R_I:  return g->i;
    case R_A:  return g->a;
    case R_E:  return g->e;
    case R_TA: return g->ta;
    case R_TE: return g->te;
    default:   return "";
    }
}

int dict_deinflect(const char *word, char out[][DICT_WORD_LEN], int max)
{
    if (word == NULL || out == NULL || max <= 0) return 0;

    const size_t lw = strlen(word);
    if (lw == 0 || lw >= DICT_WORD_LEN) return 0;

    int n = 0;

    for (int i = 0; IRREGULAR[i].from != NULL && n < max; i++)
        try_add(out, &n, max, word, lw, IRREGULAR[i].from, IRREGULAR[i].to);

    for (int i = 0; ICHIDAN_TAILS[i] != NULL && n < max; i++)
        try_add(out, &n, max, word, lw, ICHIDAN_TAILS[i], "る");

    for (int r = 0; GODAN[r].dict != NULL && n < max; r++)
        for (int f = 0; GFORM[f].tail != NULL && n < max; f++) {
            char from[24];
            snprintf(from, sizeof from, "%s%s",
                     row_kana(&GODAN[r], GFORM[f].row), GFORM[f].tail);
            try_add(out, &n, max, word, lw, from, GODAN[r].dict);
        }

    return n;
}