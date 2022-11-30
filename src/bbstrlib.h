#ifndef _BBSTRLIB_H_

#include <bstrlib.h>
#include <rvec.h>

#define BSS(s) ((struct tagbstring)bsStatic(s))

typedef rvec_t(struct tagbstring) bstrListEmb;

struct genBstrList *genBstrList_create();

int tbdelete(bstring b, int pos, int len);
int tbtrimws(bstring b);

#define bstrendswith(haystack, needle)                                         \
  (((blength(haystack) - blength(needle)) < 0)                                 \
       ? 0                                                                     \
       : (binstr((haystack), blength(haystack) - blength(needle), (needle)) == \
                  BSTR_ERR                                                     \
              ? 0                                                              \
              : 1))

#define bstrstartswith(haystack, needle) \
  (binstr((haystack), 0, (needle)) == BSTR_ERR ? 0 : 1)

struct genBstrList {
  bstring b;
  bstrListEmb *bl;
};

void bstrListEmb_destroy(bstrListEmb *l);

bstrListEmb *bsplit_noalloc(const bstring str, unsigned char splicChar);

// simpler version of bvformata -- uses predefined valist instead of variadic
// argument list
#define bvalformata(ret, b, fmt, valist)                                     \
  do {                                                                       \
    bstring bstrtmp_b = (b);                                                 \
    const char *bstrtmp_fmt = (fmt);                                         \
    int bstrtmp_r = BSTR_ERR, bstrtmp_sz = 16;                               \
    for (;;) {                                                               \
      va_list _valist;                                                       \
      va_copy(_valist, (valist));                                            \
      bstrtmp_r = bvcformata(bstrtmp_b, bstrtmp_sz, bstrtmp_fmt, (_valist)); \
      va_end(_valist);                                                       \
      if (bstrtmp_r >= 0) {                                                  \
        /* Everything went ok */                                             \
        bstrtmp_r = BSTR_OK;                                                 \
        break;                                                               \
      } else if (-bstrtmp_r <= bstrtmp_sz) {                                 \
        /* A real error? */                                                  \
        bstrtmp_r = BSTR_ERR;                                                \
        break;                                                               \
      }                                                                      \
      /* Doubled or target size */                                           \
      bstrtmp_sz = -bstrtmp_r;                                               \
    }                                                                        \
    (ret) = bstrtmp_r;                                                       \
  } while (0);

#define _BBSTRLIB_H_
#endif

bstrListEmb *bcstrsplit_noalloc(const char *str, unsigned char splicChar);
