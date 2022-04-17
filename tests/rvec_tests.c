#include "minunit.h"
#include <rvec.h>
#include <stdint.h>

static char *test_simple(void) {
    int err = 0;
    int res;
    rvec_t(int) array;
    rv_init(array);
    rv_push(array, 1, &err);
    MU_ASSERT(err == 0, "err from array");
    MU_ASSERT(rv_pop(array, &err) == 1, "Wrong value from array");
    MU_ASSERT(err == 0, "err from array");
    res = rv_pop(array, &err);
    LOG_DEBUG("Err from array is %d", err);
    MU_ASSERT(err == R_ERR_OUT_OF_BOUNDS, "Wrong err from array");
    MU_ASSERT(res == 0, "Wrong value from array");
    err = 0;
    for (int i = 0; i < 10000; i++) {
        rv_push(array, i * 333, &err);
        MU_ASSERT(err == 0, "err from array");
    }
    for (int i = 9999; i >= 0; i--) {
        res = rv_pop(array, &err);
        MU_ASSERT(err == 0, "err from array");
        MU_ASSERT(res == i * 333, "wrong value from array");
    }
    rv_destroy(array);
    return NULL;
}

static char *test_float(void) {
    int err = 0;
    int res = 0;
    float h = .54;
    float _h = 0;
    rvec_t(float) arrayy;
    rv_init(arrayy);
    rv_push(arrayy, h, &err);
    MU_ASSERT(err == 0, "err from array");
    _h = rv_pop(arrayy, &err);
    MU_ASSERT(_h == h, "Wrong value from array");
    MU_ASSERT(err == 0, "err from array");
    _h = rv_pop(arrayy, &err);
    LOG_DEBUG("Err from array is %d", err);
    MU_ASSERT(err == R_ERR_OUT_OF_BOUNDS, "Wrong err from array");
    MU_ASSERT(res == 0, "Wrong value from array");
    rv_destroy(arrayy);
    err = 0;
    return NULL;
}

struct huge {
    double i;
    long j;
};

static char *test_struct(void) {
    int err = 0;
    int res = 0;
    struct huge h = {.i = 2456, .j = 43};
    struct huge _h = {0};
    rvec_t(struct huge) arrayy;
    rv_init(arrayy);
    rv_push(arrayy, h, &err);
    MU_ASSERT(err == 0, "err from array");
    _h = (struct huge)rv_pop(arrayy, &err);
    MU_ASSERT(_h.i == 2456, "Wrong value from array");
    MU_ASSERT(err == 0, "err from array");
    _h = rv_pop(arrayy, &err);
    LOG_DEBUG("Err from array is %d", err);
    MU_ASSERT(err == R_ERR_OUT_OF_BOUNDS, "Wrong err from array");
    MU_ASSERT(res == 0, "Wrong value from array");
    err = 0;
    rv_destroy(arrayy);
    return NULL;
}

// 8443 is a magic number
static char *test_get_set_realloc(void) {
    int err = 0;
    int res = 0;
    float *f = 0;
    rvec_t(float) arrayy;
    rv_init(arrayy);
    for (int i = 0; i < 10000; i++) {
        rv_push(arrayy, (float)i, &err);
        MU_ASSERT(err == 0, "err from array");
    }

    rv_set(arrayy, 8443, (float)42, &err);
    MU_ASSERT(err == 0, "err from array");

    f = rv_get(arrayy, 8443, &err);
    MU_ASSERT(err == 0, "err from array");
    MU_ASSERT(*f == (float)42, "Wrong value from array");

    rv_resize(arrayy, 8444, &err);
    MU_ASSERT(err == 0, "err from array");
    MU_ASSERT(arrayy.m == 8444, "wrong array size");
    MU_ASSERT(arrayy.n == 8444, "wrong array size");

    for (int i = 0; i < 8443; i++) {
        f = rv_get(arrayy, i, &err);
        MU_ASSERT(err == 0, "err from array");
        MU_ASSERT(*f == (float)i, "Wrong value from array");
    }

    f = rv_get(arrayy, 8443, &err);
    MU_ASSERT(err == 0, "err from array");
    MU_ASSERT(*f == (float)42, "Wrong value from array");

    f = rv_get(arrayy, 8444, &err);
    MU_ASSERT(err == R_ERR_OUT_OF_BOUNDS, "wrong err from array");
    MU_ASSERT(f == NULL, "Wrong value from array");

    err = 0;
    rv_resize(arrayy, 10000, &err);
    MU_ASSERT(err == 0, "err from array");
    MU_ASSERT(arrayy.m == 10000, "wrong array size");
    MU_ASSERT(arrayy.n == 8444, "wrong array size");

    for (int i = 0; i < 8443; i++) {
        f = rv_get(arrayy, i, &err);
        MU_ASSERT(err == 0, "err from array");
        MU_ASSERT(*f == (float)i, "Wrong value from array");
    }
    f = rv_get(arrayy, 8443, &err);
    MU_ASSERT(err == 0, "err from array");
    MU_ASSERT(*f == (float)42, "Wrong value from array");

    err = 0;
    rv_set(arrayy, 9999, (float)0, &err);
    MU_ASSERT(err == 0, "err from array");
    for (int i = 8444; i < 10000; i++) {
        LOG_DEBUG("%d", i);
        f = rv_get(arrayy, i, &err);
        MU_ASSERT(err == 0, "err from array");
        MU_ASSERT(*f == (float)0, "Wrong value from array");
    }

    rv_set(arrayy, 10000, (float)0, &err);
    MU_ASSERT(err == R_ERR_OUT_OF_BOUNDS, "wrong err from array");

    f = rv_get(arrayy, 10000, &err);
    MU_ASSERT(err == R_ERR_OUT_OF_BOUNDS, "wrong err from array");
    MU_ASSERT(f == NULL, "Wrong value from array");

    rv_destroy(arrayy);
    err = 0;
    return NULL;
}
static char *all_tests(void) {
    MU_SUITE_START();
    MU_RUN_TEST(test_simple);
    MU_RUN_TEST(test_float);
    MU_RUN_TEST(test_struct);
    MU_RUN_TEST(test_get_set_realloc);

    return NULL;
}

RUN_TESTS(all_tests);
