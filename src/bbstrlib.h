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

#define _BBSTRLIB_H_
#endif
