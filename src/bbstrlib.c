#include <bbstrlib.h>
#include <bstrlib.h>
#include <rvec.h>
#include <stdlib.h>

static int bscb_noalloc(void *parm, int ofs, int len) {
  struct genBstrList *g = (struct genBstrList *)parm;
  struct tagbstring new_elem = {0};
  int err = 0;
  blk2tbstr(new_elem, &g->b->data[ofs], len);
  rv_push(*(g->bl), new_elem, &err);
  if (err != 0) {
    return BSTR_ERR;
  }
  return BSTR_OK;
}

bstrListEmb *bsplit_noalloc(const bstring str, unsigned char splicChar) {
  struct genBstrList gl = {0};
  int err = 0;
  if (!str) {
    return NULL;
  }
  gl.bl = calloc(1, sizeof(bstrListEmb));
  if (gl.bl == NULL) {
    return NULL;
  }
  rv_resize(*(gl.bl), 4, &err);
  if (err != 0) {
    rv_destroy(*(gl.bl));
    return NULL;
  }
  gl.b = str;
  if (bsplitcb(str, splicChar, 0, bscb_noalloc, &gl) != BSTR_OK) {
    rv_destroy(*(gl.bl));
    free(gl.bl);
    return NULL;
  }
  return gl.bl;
}

void bstrListEmb_destroy(bstrListEmb *l) {
  if (l != NULL) {
    rv_destroy(*(l));
    free(l);
  }
  return;
}

struct genBstrList *genBstrList_create() {
  struct genBstrList *ret = calloc(1, sizeof(struct genBstrList));
  int err = 0;
  if (ret == NULL) {
    return NULL;
  }
  ret->bl = calloc(1, sizeof(bstrListEmb));
  if (ret->bl == NULL) {
    free(ret);
    return NULL;
  }
  rv_resize(*(ret->bl), 4, &err);
  if (err != 0) {
    rv_destroy(*(ret->bl));
    free(ret);
    return NULL;
  }
  return ret;
}
