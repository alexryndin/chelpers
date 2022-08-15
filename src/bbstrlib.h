#ifndef _BBSTRLIB_H_

#include <bstrlib.h>
#include <rvec.h>

typedef rvec_t(struct tagbstring) bstrListEmb;

struct genBstrList *genBstrList_create();

struct genBstrList {
  bstring b;
  bstrListEmb *bl;
};

void bstrListEmb_destroy(bstrListEmb *l);

bstrListEmb *bsplit_noalloc(const bstring str, unsigned char splicChar);

// simpler version of bvformata -- uses predefined valist instead of variadic
// argument list
#define bvalformata(ret, b, fmt, valist)                                    \
  do {                                                                      \
    bstring bstrtmp_b = (b);                                                \
    const char *bstrtmp_fmt = (fmt);                                        \
    int bstrtmp_r = BSTR_ERR, bstrtmp_sz = 16;                              \
    for (;;) {                                                              \
      bstrtmp_r = bvcformata(bstrtmp_b, bstrtmp_sz, bstrtmp_fmt, (valist)); \
      if (bstrtmp_r >= 0) {                                                 \
        /* Everything went ok */                                            \
        bstrtmp_r = BSTR_OK;                                                \
        break;                                                              \
      } else if (-bstrtmp_r <= bstrtmp_sz) {                                \
        /* A real error? */                                                 \
        bstrtmp_r = BSTR_ERR;                                               \
        break;                                                              \
      }                                                                     \
      /* Doubled or target size */                                          \
      bstrtmp_sz = -bstrtmp_r;                                              \
    }                                                                       \
    ret = bstrtmp_r;                                                        \
  } while (0);

#define _BBSTRLIB_H_
#endif
