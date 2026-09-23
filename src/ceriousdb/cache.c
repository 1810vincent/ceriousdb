#include "cache.h"

#include "../dictionary/str_dictionary.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

cache* cache_init() {
    cache* new_cache = (cache*)malloc(sizeof(*new_cache));
    if (!new_cache)
        return NULL;
    new_cache->file = -1;
    new_cache->db = dict_new((size_t)0UL);
    return new_cache;
}

int32_t cache_len(cache* cache) {
    if (!cache)
        return (int32_t)-1;
    return dict_size(cache->db);
}

const char* cache_get(cache* cache, const char* key, const char* fallback) {
    if (!cache || !key)
        return NULL;

    return dict_get_fb(cache->db, key, fallback);
}

const char* cache_getall(cache* cache) {
    if (!cache)
        return NULL;
    return dict_getall(cache->db);
}

int32_t cache_insert(cache* cache, const char* key, const char* value) {
    if (!cache || !key || !value)
        return -1;
    return dict_insert(cache->db, key, value);
}

int32_t cache_remove(cache* cache, const char* key) {
    if (!cache || !key)
        return (int32_t)-1;
    return dict_remove(cache->db, key);
}

int32_t cache_load(cache* cache, const char* filepath) {
    if (!cache || !filepath)
        return -1;
    return 0;
}

int32_t cache_flush(cache* cache) {
    if (!cache)
        return -1;
    return 0;    
}

int32_t cache_release(cache* cache) {
    return (dict_free(cache->db) | close(cache->file));
}