#include "dict_kana.h"
#include "dict_types.h"
#include "dict_utf8.h"
#include <stddef.h>
#include <string.h>
int dict_kana_to_hiragana(const char *src, char *dst, size_t dst_size)
{
    if(src == NULL || dst == NULL || dst_size<=0) return DICT_ERR_ARG;
    size_t out_len = 0;
    while(*src){
        uint32_t cp;
        int n = dict_utf8_decode(src, &cp);
        if(n==0) return DICT_ERR_ARG;
        if(cp>=0x30A1 && cp<=0x30F6) cp-=0x60;
        char buf[4];
        int m = dict_utf8_encode(cp,buf);
        if(m ==0) return DICT_ERR_ARG;
        if(out_len+m+1>dst_size){
            return DICT_ERR_ARG;
        }
        memcpy(dst + out_len, buf, m);
        out_len += m;
        src += n;


    }
    dst[out_len]='\0';
    return DICT_OK;
}

DictInputType dict_kana_detect(const char *s)
{
    if (s == NULL || *s == '\0') return DICT_INPUT_UNKNOWN;

    while((*s)){
        uint32_t cp ;
        int decode = dict_utf8_decode(s,&cp);
        if(decode == 0) return DICT_INPUT_UNKNOWN;
        if ((cp >= 0x4E00 && cp <= 0x9FFF) ||
            (cp >= 0x3041 && cp <= 0x3096) ||
            (cp >= 0x30A1 && cp <= 0x30FC))
            return DICT_INPUT_JAPANESE;
        if ((cp >= 0x00C0 && cp <= 0x024F) ||
            (cp >= 0x1EA0 && cp <= 0x1EF9))
            return DICT_INPUT_VIETNAMESE;
        s+=decode;

    }
    return DICT_INPUT_LATIN;



}
