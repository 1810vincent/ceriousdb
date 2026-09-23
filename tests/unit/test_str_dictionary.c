#define TESTING_DEFINE
#include "testsuite/testing.h"
#include "../../src/dictionary/str_dictionary.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/* initializes a new str_dictionary with the default start capacity and deallocates it */
void test_dict_new_default_start_capacity_and_dict_free() {
    size_t start_capacity = (size_t)0UL;
    str_dictionary* dict = dict_new(start_capacity);
    test_assert((dict != NULL), true);
    if (dict)
        test_assert((dict->capacity == DEFAULT_START_CAPACITY), true);
    test_assert((dict_free(dict) == (int32_t)0), true);
}

/* initializes a new str_dictionary with a custom start capacity and deallocates it */
void test_dict_new_custom_start_capacity_and_dict_free() {
    size_t start_capacity = (size_t)2UL;
    str_dictionary* dict = dict_new(start_capacity);
    test_assert((dict != NULL), true);
    if (dict)
        test_assert((dict->capacity == start_capacity), true);
    test_assert((dict_free(dict) == (int32_t)0), true);
}


/* all test functions from here on initialize a new str_dictionary and deallocates it at the end implicitly (without mentioning it) */

/* test dict_insert with NULL as the str_dictionary pointer */
void test_dict_insert_dict_null() {
    test_assert((dict_insert(NULL, "key", "value") == (int32_t)-1), true);
}

/* test dict_insert with NULL as the key */
void test_dict_insert_key_null() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((dict_insert(dict, NULL, "value") == (int32_t)-1), true);
    dict_free(dict);
}

/* test dict_insert with NULL as the value */
void test_dict_insert_value_null() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((dict_insert(dict, "key", NULL) == (int32_t)-1), true);
    dict_free(dict);
}

/* test dict_insert with valid entry with new key */
void test_dict_insert_valid_new_key() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((dict_insert(dict, "key", "value") == (int32_t)0), true);
    dict_free(dict);
}

/* test dict_insert with valid entry 2 times with the same key (updates value) with dict_get */
void test_dict_insert_valid_update() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((dict_insert(dict, "key1", "value1") == (int32_t)0), true);
    test_assert((dict_insert(dict, "key1", "value2") == (int32_t)0), true);
    test_assert((strcmp(dict_get(dict, "key1"), "value2") == (int32_t)0), true);
    dict_free(dict);
}


/* test dict_remove with NULL as the str_dictionary pointer */
void test_dict_remove_dict_null() {
    test_assert((dict_remove(NULL, "key") == (int32_t)-1), true);
}

/* test dict_remove with NULL as the key */
void test_dict_remove_key_null() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((dict_remove(dict, NULL) == (int32_t)-1), true);
    dict_free(dict);
}

/* test dict_remove with invalid key */
void test_dict_remove_invalid_key() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((dict_remove(dict, "key") == (int32_t)-1), true);
    dict_free(dict);
}

/* test dict_remove with valid key */
void test_dict_remove_valid_key() {
    str_dictionary* dict = dict_new((size_t)0UL);
    dict_insert(dict, "key", "value");
    test_assert((dict_remove(dict, "key") == (int32_t)0), true);
    dict_free(dict);
}


/* test dict_contains with NULL as the str_dictionary pointer */
void test_dict_contains_dict_null() {
    test_assert((dict_contains(NULL, "key") == false), true);
}

/* test dict_contains with NULL as the key */
void test_dict_contains_key_null() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((dict_contains(dict, NULL) == false), true);
    dict_free(dict);
}

/* test dict_contains with invalid key */
void test_dict_contains_invalid_key() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((dict_contains(dict, "key") == false), true);
    dict_free(dict);
}

/* test dict_contains with valid key */
void test_dict_contains_valid_key() {
    str_dictionary* dict = dict_new((size_t)0UL);
    dict_insert(dict, "key", "value");
    test_assert((dict_contains(dict, "key") == true), true);
    dict_free(dict);
}

/* test dict_contains with (invalid) key after removing the entry with the key */
void test_dict_contains_key_after_remove() {
    str_dictionary* dict = dict_new((size_t)0UL);
    dict_insert(dict, "key", "value");
    dict_remove(dict, "key");
    test_assert((dict_contains(dict, "key") == false), true);
    dict_free(dict);
}


/* test dict_get with NULL as the str_dictionary pointer */
void test_dict_get_dict_null() {
    test_assert((dict_get(NULL, "key") == NULL), true);
}

/* test dict_get with NULL as the key */
void test_dict_get_key_null() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((dict_get(dict, NULL) == NULL), true);
    dict_free(dict);
}

/* test dict_get with invalid key */
void test_dict_get_invalid_key() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((dict_get(dict, "key") == NULL), true);
    dict_free(dict);
}

/* test dict_get with valid key */
void test_dict_get_valid_key() {
    str_dictionary* dict = dict_new((size_t)0UL);
    dict_insert(dict, "key", "value");
    test_assert((strcmp(dict_get(dict, "key"), "value") == 0), true);
    dict_free(dict);
}


/* test dict_get_fb with NULL as the str_dictionary pointer and NULL as fallback */
void test_dict_get_fb_dict_null_fallback_null() {
    test_assert((dict_get_fb(NULL, "key", NULL) == NULL), true);
}

/* test dict_get_fb with NULL as the key and NULL as fallback */
void test_dict_get_fb_key_null_fallback_null() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((dict_get_fb(dict, NULL, NULL) == NULL), true);
    dict_free(dict);
}

/* test dict_get_fb with NULL as the str_dictionary pointer and a valid fallback string */
void test_dict_get_fb_dict_null_valid_fallback() {
    test_assert((strcmp(dict_get_fb(NULL, "key", "fallback"), "fallback") == 0), true);
}

/* test dict_get_fb with NULL as the key and a valid fallback string */
void test_dict_get_fb_key_null_valid_fallback() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((strcmp(dict_get_fb(dict, NULL, "fallback"), "fallback") == 0), true);
    dict_free(dict);
}

/* test dict_get_fb with invalid key */
void test_dict_get_fb_invalid_key() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((strcmp(dict_get_fb(dict, "key", "fallback"), "fallback") == 0), true);
    dict_free(dict);
}

/* test dict_get_fb with valid key */
void test_dict_get_fb_valid_key() {
    str_dictionary* dict = dict_new((size_t)0UL);
    dict_insert(dict, "key", "value");
    test_assert((strcmp(dict_get_fb(dict, "key", "fallback"), "value") == 0), true);
    dict_free(dict);
}


/* test dict_getall with NULL as the str_dictionary pointer */
void test_dict_getall_dict_null() {
    test_assert((dict_getall(NULL) == NULL), true);
}

/* test dict_getall on an empty str_dictionary (0 entries) */
void test_dict_getall_empty_dict() {
    str_dictionary* dict = dict_new((size_t)0UL);
    char* str = dict_getall(dict);
    if (str) {
        test_assert((strcmp(str, "{}") == 0), true);
        free(str);
    }
    dict_free(dict);
}

/* test dict_getall on a str_dictionary with 1 entry */
void test_dict_getall_1entry() {
    str_dictionary* dict = dict_new((size_t)0UL);
    dict_insert(dict, "key", "value");
    char* str = dict_getall(dict);
    if (str) {
        test_assert((strcmp(str, "{ \"key\": \"value\" }") == 0), true);
        free(str);
    }
    dict_free(dict);
}

/* test dict_getall on a str_dictionary with 2 entries */
void test_dict_getall_2entries() {
    str_dictionary* dict = dict_new((size_t)0UL);
    dict_insert(dict, "key1", "value1");
    dict_insert(dict, "key2", "value2");
    char* str = dict_getall(dict);
    if (str) {
        test_assert((strcmp(str, "{ \"key1\": \"value1\", \"key2\": \"value2\" }") == 0), true);
        free(str);
    }
    dict_free(dict);
}

/* test dict_getall on a str_dictionary with 1 complex string value (json structure like) entry */
void test_dict_getall_complex_entry() {
    str_dictionary* dict = dict_new((size_t)0UL);
    dict_insert(dict, "custom dictionary", "{ dictionary: { [ key1, \"key2\" ], [ value1, \"value2\" ] } }");
    char* str = dict_getall(dict);
    if (str) {
        test_assert((strcmp(str, "{ \"custom dictionary\": \"{ dictionary: { [ key1, \"key2\" ], [ value1, \"value2\" ] } }\" }") == 0), true);
        free(str);
    }
    dict_free(dict);
}

/* test dict_getall on a str_dictionary with 1 quoted key and value strings entry */
void test_dict_getall_quoted_entry() {
    str_dictionary* dict = dict_new((size_t)0UL);
    dict_insert(dict, "\"key\"", "'value\"");
    char* str = dict_getall(dict);
    if (str) {
        test_assert((strcmp(str, "{ \"\"key\"\": \"'value\"\" }") == 0), true);
        free(str);
    }
    dict_free(dict);
}


/* test dict_size with NULL as the str_dictionary pointer */
void test_dict_size_dict_null() {
    test_assert((dict_size(NULL) == (size_t)DICT_SIZE_ERROR), true);
}

/* test dict_size on an empty str_dictionary (0 entries) */
void test_dict_size_empty_dict() {
    str_dictionary* dict = dict_new((size_t)0UL);
    test_assert((dict_size(dict) == (size_t)0UL), true);
    dict_free(dict);
}

/* test dict_size on a str_dictionary with 1 entry */
void test_dict_size_1entry() {
    str_dictionary* dict = dict_new((size_t)0UL);
    dict_insert(dict, "key", "value");
    test_assert((dict_size(dict) == (size_t)1UL), true);
    dict_free(dict);
}

/* test dict_size on a str_dictionary with 2 entries */
void test_dict_size_2entries() {
    str_dictionary* dict = dict_new((size_t)0UL);
    dict_insert(dict, "key1", "value1");
    dict_insert(dict, "key2", "value2");
    test_assert((dict_size(dict) == (size_t)2UL), true);
    dict_free(dict);
}

/* test dict_size on a str_dictionary after inserting 2 entries and removing 1 entry again */
void test_dict_size_1entry_1removed() {
    str_dictionary* dict = dict_new((size_t)0UL);
    dict_insert(dict, "key1", "value1");
    dict_insert(dict, "key2", "value2");
    dict_remove(dict, "key1");
    test_assert((dict_size(dict) == (size_t)1UL), true);
    dict_free(dict);
}


/* test capacity / size through dict_insert triggering automatic rehashing/resizing when capacity is exceeded */
void test_rehash_capacity() {
    str_dictionary* dict = dict_new((size_t)2UL);
    if (dict)
        test_assert((dict->capacity == (size_t)2UL), true);

    dict_insert(dict, "key1", "value1");
    dict_insert(dict, "key2", "value2");
    dict_insert(dict, "key3", "value3");
    dict_insert(dict, "key4", "value4");

    test_assert((dict_size(dict) == (size_t)4UL), true);
    if (dict)
        test_assert((dict->capacity >= (size_t)4UL), true);

    test_assert((strcmp(dict_get(dict, "key1"), "value1") == 0), true);
    test_assert((strcmp(dict_get(dict, "key2"), "value2") == 0), true);
    test_assert((strcmp(dict_get(dict, "key3"), "value3") == 0), true);
    test_assert((strcmp(dict_get(dict, "key4"), "value4") == 0), true);

    dict_free(dict);
}


/* test dict_insert handling DELETED slots correctly upon new insertion */
void test_deleted_slots_reuse() {
    str_dictionary* dict = dict_new((size_t)0UL);
    dict_insert(dict, "key1", "value1");
    dict_remove(dict, "key1");
    
    test_assert((dict_insert(dict, "key2", "value2") == (int32_t)0), true);
    test_assert((dict_size(dict) == (size_t)1UL), true);
    test_assert((dict_contains(dict, "key1") == false), true);
    test_assert((dict_contains(dict, "key2") == true), true);
    test_assert((strcmp(dict_get(dict, "key2"), "value2") == 0), true);

    dict_free(dict);
}


void test() {
    test_dict_new_default_start_capacity_and_dict_free();
    test_dict_new_custom_start_capacity_and_dict_free();

    test_dict_insert_dict_null();
    test_dict_insert_key_null();
    test_dict_insert_value_null();
    test_dict_insert_valid_new_key();
    test_dict_insert_valid_update();

    test_dict_remove_dict_null();
    test_dict_remove_key_null();
    test_dict_remove_invalid_key();
    test_dict_remove_valid_key();

    test_dict_contains_dict_null();
    test_dict_contains_key_null();
    test_dict_contains_invalid_key();
    test_dict_contains_valid_key();
    test_dict_contains_key_after_remove();

    test_dict_get_dict_null();
    test_dict_get_key_null();
    test_dict_get_invalid_key();
    test_dict_get_valid_key();

    test_dict_get_fb_dict_null_fallback_null();
    test_dict_get_fb_key_null_fallback_null();
    test_dict_get_fb_dict_null_valid_fallback();
    test_dict_get_fb_key_null_valid_fallback();
    test_dict_get_fb_invalid_key();
    test_dict_get_fb_valid_key();

    test_dict_getall_dict_null();
    test_dict_getall_empty_dict();
    test_dict_getall_1entry();
    test_dict_getall_2entries();
    test_dict_getall_complex_entry();
    test_dict_getall_quoted_entry();

    test_dict_size_dict_null();
    test_dict_size_empty_dict();
    test_dict_size_1entry();
    test_dict_size_2entries();
    test_dict_size_1entry_1removed();

    test_rehash_capacity();

    test_deleted_slots_reuse();
}


int main(void) {
    if (test_run(test) == UINT32_MAX)
        printf("error in test_run\n");
    return 0;
}