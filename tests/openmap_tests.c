#include "minunit.h"
#include <assert.h>
#include <openmap.h>

static int traverse_called = 0;

static int traverse_good_cb(OpenHashMapNode *node, void* userp) {
    LOG_DEBUG("KEY: %s", (char *)node->key);
    traverse_called++;
    return 0;
}

static int traverse_fail_cb(OpenHashMapNode *node, void* userp) {
    LOG_DEBUG("KEY: %s", (char *)node->key);
    traverse_called++;

    if (traverse_called == 2) {
        return 1;
    } else {
        return 0;
    }
    return -1;
}

char *test_simple() {
    int rc = 0;
    OpenHashMap *map = NULL;
    char *test1 = "test data 1";
    char *test2 = "test data 2";
    char *test3 = "test data 3";
    char *expect1 = "THE VALUE 1";
    char *expect2 = "THE VALUE 2";
    char *expect3 = "THE VALUE 3";
    map = openmap_create(NULL, NULL);
    MU_ASSERT(map != NULL, "Failed to create map.");
    MU_ASSERT(map != NULL, "Failed to create map.");
    MU_ASSERT(map->hash != NULL, "Failed to create map.");
    MU_ASSERT(map->compare != NULL, "Failed to create map.");

    rc = openmap_set(map, test1, expect1);
    MU_ASSERT(rc == 0, "Failed to set &test1");
    char *result = openmap_get(map, test1);
    MU_ASSERT(!strcmp(result, expect1), "Wrong value for test1.");

    rc = openmap_set(map, test2, expect2);
    MU_ASSERT(rc == 0, "Failed to set &test2");
    result = openmap_get(map, test2);
    MU_ASSERT(!strcmp(result, expect2), "Wrong value for test2.");

    rc = openmap_set(map, test3, expect3);
    MU_ASSERT(rc == 0, "Failed to set &test3");
    result = openmap_get(map, test3);
    MU_ASSERT(!strcmp(result, expect3), "Wrong value for test3.");

    rc = openmap_traverse(map, traverse_good_cb, NULL);
    MU_ASSERT(rc == 0, "Failed to traverse.");
    MU_ASSERT(traverse_called == 3, "Wrong count traverse.");

    traverse_called = 0;
    rc = openmap_traverse(map, traverse_fail_cb, NULL);
    MU_ASSERT(rc == -1, "Failed to traverse.");
    MU_ASSERT(traverse_called == 2, "Wrong count traverse for fail.");

    char *deleted = openmap_delete(map, test1);
    MU_ASSERT(deleted != NULL, "Got NULL on delete.");
    MU_ASSERT(!strcmp(deleted,expect1), "Should get test1.");

    result = openmap_get(map, &test1);
    MU_ASSERT(result == NULL, "Should delete.");

    deleted = openmap_delete(map, test2);
    MU_ASSERT(deleted != NULL, "Got NULL on delete.");
    MU_ASSERT(!strcmp(deleted,expect2), "Should get test2.");

    result = openmap_get(map, test2);
    MU_ASSERT(result == NULL, "Should delete.");

    deleted = openmap_delete(map, test3);
    MU_ASSERT(deleted != NULL, "Got NULL on delete.");
    MU_ASSERT(!strcmp(deleted,expect3), "Should get test3.");

    result = openmap_get(map, &test3);
    MU_ASSERT(result == NULL, "Should delete.");

    openmap_destroy(map);

    return NULL;
}

char *test_uint32() {
    uint32_t i = 0;
    int rc = 0;
    uint32_t *key = NULL, *value = NULL;
    OpenHashMap *map = NULL;
    map = openmap_create(
        (HashMap_compare)uint32_cmp, (HashMap_hash)uint32_hash);
    for (i = 0; i < 10000; i++) {
        key = malloc(sizeof(uint32_t));
        value = malloc(sizeof(uint32_t));
        CHECK_MEM(key);
        CHECK_MEM(value);
        *value = i * 333;
        *key = i;
        rc = openmap_set(map, key, value);
        MU_ASSERT(rc == 0, "Failed to insert into map");
    }

    for (i = 0; i < 10000; i++) {
        value = openmap_get(map, &i);
        MU_ASSERT(*value == i * 333, "Failed to get from map");
    }

    LOG_DEBUG("number of buckets %zu", map->number_of_buckets);
    LOG_DEBUG("number of elements %zu", map->number_of_elements);
    MU_ASSERT(openmap_destroy_with_kv(map) == 0, "Failed to destroy map.");
error:
    return NULL;
}

char *all_tests() {
    MU_SUITE_START();

    MU_RUN_TEST(test_simple);
    MU_RUN_TEST(test_uint32);

    return NULL;
}

RUN_TESTS(all_tests);
