#include "str_dictionary.h"


#define XXH_INLINE_ALL
#include "external/xxhash.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


str_dictionary* dict_new(size_t capacity) {
    if (capacity == (size_t)0UL)
        capacity = DEFAULT_START_CAPACITY;

    str_dictionary* dict = (str_dictionary*)malloc(sizeof(*dict));
    if (!dict)
        return NULL;

    dict->size = 0UL;
    dict->capacity = capacity;
    dict->entries = (entry*)malloc(dict->capacity * sizeof(*(dict->entries)));
    if (!(dict->entries)) {
        free(dict);
        dict = NULL;
        return NULL;
    }

    for (size_t i = (size_t)0UL; i < dict->capacity; i++) {
        dict->entries[i].key = NULL;
        dict->entries[i].value = NULL;
        dict->entries[i].state = EMPTY;
    }

    return dict;
}


size_t dict_size(const str_dictionary* dict) {
    if (!dict)
        return DICT_SIZE_ERROR;
    return dict->size;
}


/*
* xxh3 64bit hash algorithm (from file 'src/dictionary/external/xxhash.h' 
* by 'https://xxhash.com/' / 'https://github.com/cyan4973/xxhash')
*/
static inline uint64_t hash(const char* key) {
    return XXH3_64bits(key, strlen(key));
}

static inline int64_t get_hash_index(const str_dictionary* dict, const char* key) {
    if (!dict || !key || (dict->capacity == (size_t)0UL))
        return (int64_t)-1L;
    return (int64_t)(hash(key) % dict->capacity);
}

static int64_t find_get_index(const str_dictionary* dict, const char* key) {
    if (!dict || !key || (dict->capacity == (size_t)0UL))
        return (int64_t)-1L;

    int64_t hash_index_ret = get_hash_index(dict, key);
    if (hash_index_ret == (int64_t)-1L)
        return (int64_t)-1L;
    size_t hash_index = (size_t)hash_index_ret;
    size_t index = hash_index;

    do {
        if (dict->entries[index].state == EMPTY)
            return (int64_t)-1L;

        if (dict->entries[index].state == FILLED) {
            if (strcmp(dict->entries[index].key, key) == 0)
                return (int64_t)index;
        }

        index = (index + 1UL) % dict->capacity;
    } while (index != hash_index);

    return (int64_t)-1L;
}


bool dict_contains(const str_dictionary* dict, const char* key) {
    if (!dict || !key || (dict->capacity == (size_t)0UL))
        return false;

    return (find_get_index(dict, key) != (int64_t)-1L);
}


const char* dict_get(const str_dictionary* dict, const char* key) {
    if (!dict || !key || (dict->capacity == (size_t)0UL))
        return NULL;

    int64_t index = find_get_index(dict, key);
    if (index == (int64_t)-1L)
        return NULL;
    return dict->entries[(size_t)index].value;
}

const char* dict_get_fb(const str_dictionary* dict, const char* key, const char* fallback) {
    if (!dict || !key || (dict->capacity == (size_t)0UL))
        return fallback;

    const char* value = dict_get(dict, key);
    if (!value)
        return fallback;
    return value;
}

char* dict_getall(const str_dictionary* dict) {
    if (!dict)
        return NULL;

    /* if dict is empty -> return empty json object string "{}" */
    if (dict->size == (size_t)0UL) {
        const char* str_empty = "{}";
        char* str = (char*)malloc(strlen(str_empty) + 1);
        if (!str)
            return NULL;
        strcpy(str, str_empty);
        return str;
    }

    /* determine needed json object string length in bytes for malloc */
    /* base empty size: 5 bytes: "{  }\0" */
    size_t len = (size_t)5UL;
    bool first_entry = true;
    for (size_t i = (size_t)0UL; i < dict->capacity; i++) {
        if (dict->entries[i].state == FILLED) {
            /* 2 more bytes after first entry for separator: ", " */
            if (!first_entry)
                len += 2UL;
            if (first_entry)
                first_entry = false;
            /* bytes: 1->'"' + strlen->key + 2->'":' + 2->' "' + strlen->value + 1->'"' */
            len += (1 + strlen(dict->entries[i].key) + 2 + 2 + strlen(dict->entries[i].value) + 1);
        }
    }

    char* str = (char*)malloc(len);
    if (!str)
        return NULL;

    /* printf json object string into buffer */
    char* write_ptr = str;
    size_t remaining = len;

    int32_t written = (int32_t)snprintf(write_ptr, remaining, "{ ");
    if (written < 0)
        return NULL;
    write_ptr += written;
    remaining -= (size_t)written;

    first_entry = true;
    for (size_t i = (size_t)0UL; i < dict->capacity; i++) {
        if (dict->entries[i].state == FILLED) {
            if (!first_entry) {
                written = (int32_t)snprintf(write_ptr, remaining, ", ");
                if (written < 0) {
                    free(str);
                    return NULL;
                }
                write_ptr += written;
                remaining -= (size_t)written;
            }

            if (first_entry)
                first_entry = false;

            written = (int32_t)snprintf(write_ptr, remaining, "\"%s\": \"%s\"", dict->entries[i].key, dict->entries[i].value);
            if (written < 0) {
                free(str);
                return NULL;
            }
            write_ptr += written;
            remaining -= (size_t)written;
        }
    }

    written = (int32_t)snprintf(write_ptr, remaining, " }");
    if (written < 0) {
        free(str);
        return NULL;
    }

    return str;
}


static inline double get_loadfactor(const str_dictionary* dict) {
    if (!dict || dict->capacity == (size_t)0UL)
        return (double)-1.0;

    double occupied = (double)0.0;
    for (size_t i = (size_t)0UL; i < dict->capacity; i++) {
        if ((dict->entries[i].state == FILLED) || dict->entries[i].state == DELETED)
            occupied++;
    }
    return (occupied / (double)dict->capacity);
}

static int32_t resize(str_dictionary* dict) {
    if (!dict || (dict->capacity == (size_t)0UL))
        return (int32_t)-1;

    size_t new_capacity = RESIZE_FACTOR * dict->capacity;
    /* int32_t overflow protection (if new_capacity wrapped) */
    if (new_capacity < dict->capacity)
        return (int32_t)-1;

    str_dictionary* new_dict = dict_new(new_capacity);
    if (!new_dict)
        return (int32_t)-1;

    /* rehash all entries of dict and insert in new_dict */
    for (size_t i = (size_t)0UL; i < dict->capacity; i++) {
        if (dict->entries[i].state == FILLED) {
            if (dict_insert(new_dict, dict->entries[i].key, dict->entries[i].value) == (int32_t)-1) {
                dict_free(new_dict);
                return (int32_t)-1;
            }
        }
    }

    /* deallocate all entries (including keys & values) of the old dict, but not the dict (pointer) itself */
    for (size_t i = (size_t)0UL; i < dict->capacity; i++) {
        if (dict->entries[i].state == FILLED) {
            free(dict->entries[i].key);
            free(dict->entries[i].value);
        }
    }
    free(dict->entries);

    /* insert new entries & new capacity from new_dict into dict (size stays the same) */
    dict->entries = new_dict->entries;
    dict->capacity = new_dict->capacity;
    
    free(new_dict);

    return (int32_t)0;
}

int32_t dict_insert(str_dictionary* dict, const char* key, const char* value) {
    if (!dict || !key || !value || (dict->capacity == (size_t)0UL))
        return (int32_t)-1;

    double loadfactor = get_loadfactor(dict);
    if (loadfactor < (double)0.0)
        return (int32_t)-1;

    /* check load factor and resize (& rehash) if needed */
    if (loadfactor >= MAX_LOAD_FACTOR)
        if (resize(dict) == (int32_t)-1)
            return (int32_t)-1;

    int64_t hash_index_ret = get_hash_index(dict, key);
    if (hash_index_ret == (int64_t)-1L)
        return (int64_t)-1L;
    size_t hash_index = (size_t)hash_index_ret;
    size_t index = hash_index;

    int64_t first_deleted = (int64_t)-1;

    /* linear probing */
    do {
        entry_state state = dict->entries[index].state;

        if (state == EMPTY) {
            if (first_deleted != (int64_t)-1)
                index = (size_t)first_deleted;
            break;
        }
        else if (state == DELETED) {
            if (first_deleted == (int64_t)-1)
                first_deleted = index;
        }
        else if (state == FILLED) {
            /* check if entry with key already exists -> update value */
            if (strcmp(dict->entries[index].key, key) == 0) {
                free(dict->entries[index].value);

                size_t value_len = strlen(value);
                dict->entries[index].value = (char*)malloc(value_len + 1);
                if (!dict->entries[index].value)
                    return (int32_t)-1;
                strncpy(dict->entries[index].value, value, value_len + 1);

                return (int32_t)0;
            }
        }

        index = (index + 1) % dict->capacity;

    } while (index != hash_index);

    /* if dict is full (should not happen) */
    if (dict->entries[index].state == FILLED && first_deleted == (int64_t)-1) {
        return (int32_t)-1;
    }

    /* if first_deleted is found but no empty slot */
    if (first_deleted != (int64_t)-1 && dict->entries[index].state != EMPTY) {
        index = (size_t)first_deleted;
    }

    /* create new entry with key & value */
    dict->entries[index].state = FILLED;

    size_t key_len = strlen(key);
    dict->entries[index].key = (char*)malloc(key_len + 1);
    if (!dict->entries[index].key)
        return (int32_t)-1;
    strncpy(dict->entries[index].key, key, key_len + 1);

    size_t value_len = strlen(value);
    dict->entries[index].value = (char*)malloc(value_len + 1);
    if (!dict->entries[index].value) {
        free(dict->entries[index].key);
        dict->entries[index].key = NULL;
        return (int32_t)-1;
    }
    strncpy(dict->entries[index].value, value, value_len + 1);

    dict->size++;

    return (int32_t)0;
}


int32_t dict_remove(str_dictionary* dict, const char* key) {
    if (!dict || !key || (dict->size == (size_t)0UL))
        return (int32_t)-1;

    int64_t index = find_get_index(dict, key);
    if (index == (int64_t)-1)
        return (int32_t)-1;
    index = (size_t)index;

    dict->entries[index].state = DELETED;

    free(dict->entries[index].key);
    dict->entries[index].key = NULL;

    free(dict->entries[index].value);
    dict->entries[index].value = NULL;

    dict->size--;

    return (int32_t)0;
}


int32_t dict_free(str_dictionary* dict) {
    if (!dict)
        return (int32_t)-1;
    
    for (size_t i = (size_t)0UL; i < dict->capacity; i++) {
        if (dict->entries[i].state == FILLED) {
            free(dict->entries[i].key);
            dict->entries[i].key = NULL;
            free(dict->entries[i].value);
            dict->entries[i].value = NULL;
        }
    }

    free(dict->entries);
    dict->entries = NULL;

    free(dict);
    dict = NULL;
    
    return (int32_t)0;
}