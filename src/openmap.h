#ifndef OPENMAP_H
#define OPENMAP_H

#include <dbg.h>
#include <rvec.h>
#include <stdint.h>

#define DEFAULT_NUMBER_OF_BUCKETS 100
#define DEFAULT_REALLOC_FACTOR    0.9
#define DEFAULT_EXPAND_FACTOR    1.5

typedef int (*OpenHashMap_compare)(void *a, void *b);
typedef uint32_t (*OpenHashMap_hash)(void *key);

typedef int (*OpenHashMap_compare)(void *a, void *b);
typedef uint32_t (*OpenHashMap_hash)(void *key);

#define IS_VACANT(n) ((n)->data == NULL || (n)->key == NULL || (n)->deleted)

typedef struct OpenHashMapNode {
    void *key;
    void *data;
    uint32_t hash;
    char deleted;
} OpenHashMapNode;

typedef rvec_t(OpenHashMapNode) OpenHashMapNodeVec;

typedef struct OpenHashMap {
    OpenHashMapNodeVec buckets;
    OpenHashMap_compare compare;
    OpenHashMap_hash hash;
    size_t number_of_buckets;
    size_t number_of_elements;
    double realloc_factor;
    double expand_factor;
} OpenHashMap;

#define ByteOf(a, b) (((uint8_t *)a)[(b)])

#define JENKINS_HASH(ret, len, value, accessor) \
    for (ret = i = 0; i < (len); i++) {         \
        ret += accessor(value, i);              \
        ret += (ret << 10);                     \
        ret ^= (ret >> 6);                      \
    }                                           \
    ret += (ret << 3);                          \
    ret ^= (ret >> 11);                         \
    ret += (ret << 15)

typedef int (*HashMap_compare)(void *a, void *b);
typedef uint32_t (*HashMap_hash)(void *key);

int uint32_cmp(uint32_t *a, uint32_t *b);
uint32_t uint32_hash(uint32_t *key);


int cstr_cmp(char* b1, char* b2);
uint32_t cstr_hash(const char* key);

typedef int (*OpenHashMap_traverse_cb)(OpenHashMapNode *node, void* userdata);

OpenHashMap *
openmap_create(OpenHashMap_compare compare, OpenHashMap_hash hash);

void openmap_destroy(OpenHashMap *map);

int openmap_destroy_with_kv(OpenHashMap *map);

int openmap_set(OpenHashMap *map, void *key, void *data);
void *openmap_get(OpenHashMap *map, void *key);

int openmap_traverse(OpenHashMap *map, OpenHashMap_traverse_cb traverse_cb, void *userdata);

void *openmap_delete(OpenHashMap *map, void *key);

#endif
