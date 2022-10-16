#include <bbstrlib.h>
#include <bstrlib.h>
#include <rvec.h>
#include <stdlib.h>

#define wspace(c) (isspace((unsigned char)c))

/* Just a length safe wrapper for memmove. */

#define bBlockCopy(D, S, L)   \
  do {                        \
    if ((L) > 0) {            \
      memmove((D), (S), (L)); \
    }                         \
  } while (0);

int tbdelete(bstring b, int pos, int len) {
  /* Clamp to left side of bstring */
  if (pos < 0) {
    len += pos;
    pos = 0;
  }
  if (len < 0 || b == NULL || b->data == NULL || b->slen < 0) {
    return BSTR_ERR;
  }
  if (len > 0 && pos < b->slen) {
    if (pos + len >= b->slen) {
      b->slen = pos;
    } else {
      bBlockCopy(
          (char *)(b->data + pos),
          (char *)(b->data + pos + len),
          b->slen - (pos + len));
      b->slen -= len;
    }
    b->data[b->slen] = (unsigned char)'\0';
  }
  return BSTR_OK;
}

int tbtrimws(bstring b) {
  int i, j;
  if (b == NULL || b->data == NULL || b->slen < 0) {
    return BSTR_ERR;
  }
  for (i = b->slen - 1; i >= 0; i--) {
    if (!wspace(b->data[i])) {
      if (b->mlen > i) {
        b->data[i + 1] = (unsigned char)'\0';
      }
      b->slen = i + 1;
      for (j = 0; wspace(b->data[j]); j++)
        ;
      return tbdelete(b, 0, j);
    }
  }
  b->data[0] = (unsigned char)'\0';
  b->slen = 0;
  return BSTR_OK;
}

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

bstrListEmb *bcstrsplit_noalloc(const char *str, unsigned char splicChar) {
  if (str == NULL) {
    return NULL;
  }
  struct tagbstring tb = {0};
  cstr2tbstr(tb, str);
  return bsplit_noalloc(&tb, splicChar);
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
