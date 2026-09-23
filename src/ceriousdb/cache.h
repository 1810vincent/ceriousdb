#ifndef CERIOUSDB_CACHE_H
#define CERIOUSDB_CACHE_H

#include "../dictionary/str_dictionary.h"
#include <stdint.h>

typedef struct {
    str_dictionary* db;
    int file;
} cache;

cache* cache_init();

int32_t cache_len(cache* cache);

const char* cache_get(cache* cache, const char* key, const char* fallback);

const char* cache_getall(cache* cache);

int32_t cache_insert(cache* cache, const char* key, const char* value);

int32_t cache_remove(cache* cache, const char* key);

int32_t cache_load(cache* cache, const char* filepath);

int32_t cache_flush(cache* cache);

int32_t cache_release(cache* cache);

#endif