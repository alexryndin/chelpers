#ifndef RVEC_H
#define RVEC_H

#include "dbg.h"
#include <stdlib.h>

typedef enum {
  R_ERR_MEM = -1,
  R_ERR_NP = -2,
  R_ERR_OUT_OF_BOUNDS = -3,
  R_ERR_VALUE = -4,
} R_EC;

#define rvec_t(t) \
  struct {        \
    size_t n;     \
    size_t m;     \
    t *a;         \
  }

#define rv_init(v)  ((v).n = (v).m = 0, (v).a = NULL)
#define rv_at(v, i) ((v).a[(i)])
// #define rv_pop(v)     ((v).a[--(v).n])
#define rv_len(v) ((v).n)
#define rv_max(v) ((v).m)

#define REALLOC_FACTOR 1.5
#define rv_destroy(v)    \
  do {                   \
    if ((v).a != NULL) { \
      free((v).a);       \
      (v).a = NULL;      \
      (v).m = 0;         \
      (v).n = 0;         \
    }                    \
  } while (0)

#define rv_resize(v, s, e)                                        \
  do {                                                            \
    if (s < 1) {                                                  \
      LOG_ERR("Wrong realloc value");                             \
      if (e != NULL) {                                            \
        *e = R_ERR_VALUE;                                         \
      }                                                           \
      break;                                                      \
    }                                                             \
    typeof(*(v).a) *swap = NULL;                                  \
    swap = realloc((v).a, sizeof(*(v).a) * s);                    \
    if (swap == NULL) {                                           \
      LOG_ERR("Vector reallocation failed");                      \
      if (e != NULL) {                                            \
        *e = R_ERR_MEM;                                           \
      }                                                           \
      break;                                                      \
    }                                                             \
    (v).a = swap;                                                 \
    (v).m = (s);                                                  \
    if (s <= (v).n) {                                             \
      (v).n = s;                                                  \
    } else {                                                      \
      memset((v).a + (v).n, 0, ((v).m - (v).n) * sizeof(*(v).a)); \
    }                                                             \
  } while (0)

#define rv_push(vec, value, err)                                    \
  do {                                                              \
    void *swap;                                                     \
    size_t new_length;                                              \
    if ((vec).n == (vec).m) {                                       \
      new_length = (vec).m <= 0 ? 2 : (vec).m * REALLOC_FACTOR + 1; \
      swap = realloc((vec).a, sizeof(*(vec).a) * new_length);       \
      if (swap == NULL) {                                           \
        LOG_ERR("Vector reallocation failed");                      \
        if (err != NULL) {                                          \
          *(int *)err = R_ERR_MEM;                                  \
        }                                                           \
      }                                                             \
      (vec).a = swap;                                               \
      (vec).m = new_length;                                         \
    }                                                               \
    (vec).a[(vec).n++] = (value);                                   \
  } while (0)

#define rv_pop(v, e)                                                         \
  (((v).n <= (v).m) && ((v).n > 0)                                           \
       ? (v).a[--(v).n]                                                      \
       : (e != NULL ? (*(int *)e = R_ERR_OUT_OF_BOUNDS, (typeof(*(v).a)){0}) \
                    : (typeof(*(v).a)){0},                                   \
          (typeof(*(v).a)){0}))

#define rv_get(v, i, e)                                                       \
  (((i) < (v).n) && ((i) >= 0)                                                \
       ? &((v).a[i])                                                          \
       : (e != (NULL)                                                         \
              ? (*(int *)e = R_ERR_OUT_OF_BOUNDS, ((typeof(*(v).a) *)(NULL))) \
              : ((typeof(*(v).a) *)(NULL))))

#define rv_set(vec, i, value, e)                                               \
  (((i) < (vec).m) && ((i) >= 0)                                               \
       ? ((vec).n = ((vec).n > i ? (vec).n : i + 1), (vec).a[i] = value)       \
       : (e != NULL ? (*(int *)e = R_ERR_OUT_OF_BOUNDS, (typeof(*(vec).a)){0}) \
                    : (typeof(*(vec).a)){0},                                   \
          (typeof(*(vec).a)){0}))

#endif
