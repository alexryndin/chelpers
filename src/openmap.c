#include "openmap.h"
#include "rvec.h"

int str_cmp(char *b1, char *b2) { return strcmp(b1, b2); }

uint32_t str_hash(const char *key) {
    uint32_t ret, i;

    JENKINS_HASH(ret, strlen(key), key, ByteOf);
    return ret;
}

int uint32_cmp(uint32_t *a, uint32_t *b) {
    if (*a < *b)
        return -1;
    if (*a > *b)
        return 1;
    return 0;
}

uint32_t uint32_hash(uint32_t *num) {
    uint32_t ret, i;

    JENKINS_HASH(ret, 4, num, ByteOf);
    return ret;
}

OpenHashMap *_openmap_create(
    OpenHashMap_compare compare,
    OpenHashMap_hash hash,
    size_t number_of_buckets) {
    OpenHashMap *ret = calloc(1, sizeof(OpenHashMap));
    int err = 0;
    CHECK_MEM(ret);

    ret->compare = compare != NULL ? compare : (OpenHashMap_compare)str_cmp;
    ret->hash = hash != NULL ? hash : (OpenHashMap_hash)str_hash;
    ret->number_of_buckets = number_of_buckets;
    ret->realloc_factor = DEFAULT_REALLOC_FACTOR;
    ret->expand_factor = DEFAULT_EXPAND_FACTOR;
    rv_init(ret->buckets);
    rv_resize(ret->buckets, ret->number_of_buckets, &err);
    CHECK(err == 0, "Couldn't initialize buckets");
    // hack to set end to max
    rv_set(
        ret->buckets, ret->number_of_buckets - 1, (OpenHashMapNode){0}, NULL);
    CHECK(
        rv_max(ret->buckets) == ret->number_of_buckets,
        "Couldn't initialize buckets.");
    CHECK(
        rv_len(ret->buckets) == ret->number_of_buckets,
        "Couldn't initialize buckets.");
    CHECK(ret->number_of_buckets > 50, "Couldn't initialize map.");
    CHECK(ret->expand_factor > 1., "Couldn't initialize map.");
    CHECK(ret->realloc_factor < 1., "Couldn't initialize map.");
    return ret;
error:
    if (ret != NULL) {
        rv_destroy(ret->buckets);
        free(ret);
    }
    return NULL;
}

OpenHashMap *
openmap_create(OpenHashMap_compare compare, OpenHashMap_hash hash) {
    return _openmap_create(compare, hash, DEFAULT_NUMBER_OF_BUCKETS);
}

int openmap_destroy_with_kv(OpenHashMap *map) {
    int rc = 0;
    OpenHashMapNode *node = NULL;
    CHECK(map != NULL, "Null map.");
    for (size_t i = 0; i < map->number_of_buckets; i++) {
        node = rv_get(map->buckets, i, NULL);
        if (node == NULL) {
            continue;
        };
        if (node->key != NULL) {
            free(node->key);
            node->key = NULL;
        }
        if (node->data != NULL) {
            free(node->data);
            node->data = NULL;
        }
    }
    rv_destroy(map->buckets);
    free(map);
error:
    return rc;
}

void openmap_destroy(OpenHashMap *map) {
    CHECK(map != NULL, "Null map.");
    if (rv_len(map->buckets) != map->number_of_buckets)
        LOG_ERR("number of buckets != len of buckets array.");
    rv_destroy(map->buckets);
    free(map);
error:
    return;
}

int openmap_realloc(OpenHashMap *map) {
    LOG_DEBUG(
        "Reallocating map, number of buckets %zu, expand factor %f.",
        map->number_of_buckets,
        map->expand_factor);
    int rc = 0;
    OpenHashMap *new = NULL;
    CHECK(map != NULL, "Cannot reallocate null map.");
    new = _openmap_create(
        map->compare,
        map->hash,
        (size_t)(map->expand_factor * map->number_of_buckets));
    CHECK(new != NULL, "Couldn't create map to reallocate old.");

    OpenHashMapNode *node = NULL;
    for (size_t i = 0; i < map->number_of_buckets; i++) {
        node = rv_get(map->buckets, i, NULL);
        if (node == NULL) {
            continue;
        }
        if (!IS_VACANT(node)) {
            rc = openmap_set(new, node->key, node->data);
            CHECK(rc == 0, "Couldn't add node to a new map.");
        }
    }

    // new map is created, now we use its guts to expand old map
    // :)
    OpenHashMapNodeVec swap = new->buckets;
    new->buckets = map->buckets;
    map->buckets = swap;

    size_t swap_n = new->number_of_buckets;
    new->number_of_buckets = map->number_of_buckets;
    map->number_of_buckets = swap_n;

error:
    if (new != NULL) {
        openmap_destroy(new);
    }
    return rc;
}
int openmap_set(OpenHashMap *map, void *key, void *data) {
    CHECK(map != NULL, "Null map.");
    char ok = 0;
    uint32_t hash = map->hash(key);
    int rc = 0;
    size_t n_bucket = hash % map->number_of_buckets;
    size_t i = n_bucket;
    OpenHashMapNode *node = NULL;
    // LOG_DEBUG("Num of bucket %zu, num of buckets %zu", n_bucket,
    // map->number_of_buckets);
    do {
        node = rv_get(map->buckets, i, NULL);
        CHECK(node != NULL, "Failed to get node from array");
        if (IS_VACANT(node)) {
            node->data = data;
            node->hash = hash;
            node->key = key;
            node->deleted = 0;
            map->number_of_elements++;
            ok = 1;
        }
        i = (i + 1) % map->number_of_buckets;

    } while (i != n_bucket && !ok);

    if (map->number_of_elements >
        map->number_of_buckets * map->realloc_factor) {
        rc = openmap_realloc(map);
        CHECK(rc == 0, "Couldn't reallocate map.");
    }

    if (!ok) {
        LOG_ERR("Map seems to be full");
        goto error;
    }

exit:
    return rc;
error:
    rc = -1;
    goto exit;
}

void *openmap_get(OpenHashMap *map, void *key) {
    void *ret = NULL;
    CHECK(map != NULL, "Null map.");
    uint32_t hash = map->hash(key);
    size_t n_bucket = hash % map->number_of_buckets;
    size_t i = n_bucket;
    OpenHashMapNode *node = NULL;
    do {
        node = rv_get(map->buckets, i, NULL);
        CHECK(node != NULL, "Failed to get node from array");
        if (IS_VACANT(node)) {
            goto exit;
        }
        if (hash == node->hash && map->compare(key, node->key) == 0) {
            ret = node->data;
            goto exit;
        }
        i = (i + 1) % map->number_of_buckets;

    } while (i != n_bucket);

exit:
error:
    return ret;
}

int openmap_traverse(
    OpenHashMap *map, OpenHashMap_traverse_cb traverse_cb, void *userdata) {
    int rc = 0;
    size_t i = 0;
    CHECK(map != NULL, "Null map.");
    OpenHashMapNode *node = NULL;
    for (i = 0; i < map->number_of_buckets; i++) {
        node = rv_get(map->buckets, i, NULL);
        if (node == NULL) {
            continue;
        }
        if (!IS_VACANT(node)) {
            rc = traverse_cb(node, userdata);
            CHECK(rc == 0, "Callback failed.");
        }
    }

exit:
    return rc;
error:
    rc = -1;
    goto exit;
}

void *openmap_delete(OpenHashMap *map, void *key) {
    void *ret = NULL;
    uint32_t hash = map->hash(key);
    size_t n_bucket = hash % map->number_of_buckets;
    size_t i = n_bucket;
    OpenHashMapNode *node = NULL;
    do {
        node = rv_get(map->buckets, i, NULL);
        CHECK(node != NULL, "Couldn't find bucket.");
        LOG_DEBUG("bucket n %lu, key %s", hash % map->number_of_buckets, key);
        if (node->data == NULL || node->key == NULL || node->deleted) {
            goto exit;
        }
        if (hash == node->hash && map->compare(key, node->key) == 0) {
            ret = node->data;
            node->data = NULL;
            node->key = NULL;
            node->deleted = 1;
            goto exit;
        }
        i = (i + 1) % map->number_of_buckets;

    } while (i != n_bucket);

exit:
error:
    return ret;
}
