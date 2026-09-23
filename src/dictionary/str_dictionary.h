/**
* @file str_dictionary.h
*
* @brief efficient (python-like) dictionary (hashmap) implementation
*        for string-only keys and string-only values
*
* @note this dictionary implementation is specifically made & optimized
*       for usage as an in-memory datastructure in a string key:value database
*
* @note the current implementation uses basic heap allocation (malloc) for
*       the dictionary itself and all entries including the key and value.      
*       in the future this will change to the usage of an arena heap allocator.
*
* @warning key & value can not be NULL! to use an empty key or value
*          (e.g. to insert), pass an empty string ("") as key or value
*
* @warning caller responsibility: in all functions with a str_dictionary
*          pointer as a parameter (str_dictionary* dict), the pointer
*          has to point to valid, allocated memory (or NULL)!
*          if dict points to invalid memory -> guaranteed segmentation fault
*          if dict is a NULL pointer -> is handled by all functions
*
* @author 1810vincent (val <3)
*
* @date 09/2026
*/
#ifndef C_STR_DICTIONARY_H
#define C_STR_DICTIONARY_H


#if defined(__GNUC__) || defined(__CLANG__)
    #define attr_malloc __attribute__((malloc))
#else
    #define attr_malloc
#endif


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


#define DEFAULT_START_CAPACITY (size_t)64UL

#define MAX_LOAD_FACTOR (double)0.75

#define RESIZE_FACTOR (size_t)2UL

#define DICT_SIZE_ERROR SIZE_MAX


typedef enum {
    EMPTY,
    FILLED,
    DELETED
} entry_state;

typedef struct {
    char* key;
    char* value;
    entry_state state;
} entry;

typedef struct {
    entry* entries;
    size_t capacity;
    size_t size;
} str_dictionary;


/**
* @brief allocates and initializes a new str_dictionary with
*        capacity as the initial capacity, or DEFAULT_START_CAPACITY
*        (default: 64) if 0 is passed as capacity
*
* @param[in] capacity   the initial capacity, defaults to DEFAULT_START_CAPACITY
*                       (default: 64) if 0 is passed
*
* @return returns the new str_dictionary on success, or NULL on error
*/
str_dictionary* dict_new(size_t capacity);


/**
* @brief returns the current size (amount of entries) of dict
*
* @param[in] dict   the str_dictionary
*
* @return returns the current amount of entries in dict on success,
*         or DICT_SIZE_ERROR (on 64-bit: 0xffffffffffffffff) on error
*/
size_t dict_size(const str_dictionary* dict);


/**
* @brief returns whether dict contains an entry with key
*
* @param[in] dict   the str_dictionary
* @param[in] key    the key
*
* @return returns true if dict contains an entry with key, or false if
*         it does not contain an entry with key or an error occurs
*/
bool dict_contains(const str_dictionary* dict, const char* key);


/**
* @brief returns the value of the entry with key from dict
*
* @param[in] dict   the str_dictionary
* @param[in] key    the key
*
* @warning since the return value is a pointer to the string in dict,
*          if dict gets resized & rehashed by a following dict_insert call,
*          the pointer will point to another value or invalid memory!
*          accessing this pointer is undefined behaviour!
*          to avoid this, copy the value into a local buffer or only
*          use it before calling dict_insert afterwards.
*
* @return returns the value of the entry with key on success,
*         or NULL on error
*/
const char* dict_get(const str_dictionary* dict, const char* key);

/**
* @brief returns the value of the entry with key from dict, or fallback on error
*
* @param[in] dict       the str_dictionary
* @param[in] key        the key
* @param[in] fallback   the fallback value string
*
* @warning since the return value is a pointer to the string in dict,
*          if dict gets resized & rehashed by a following dict_insert call,
*          the pointer will point to another value or invalid memory!
*          accessing this pointer is undefined behaviour!
*          to avoid this, copy the value into a local buffer or only
*          use it before calling dict_insert afterwards.
*
* @return returns the value of the entry with key on success,
*         or the fallback value string on error
*/
const char* dict_get_fb(const str_dictionary* dict, const char* key, const char* fallback);


/**
* @brief gets a newly heap-allocated json object string containing all entries in dict
*
* @details returns a string of all entries in dict in the following
*          json object schema:
*          '{ "key1": "value1", "key2": "value2" }'
*          if dict is empty the following empty json object string will get returned:
*          '{}'
*
* @warning returning string has caller-ownership: the returning string is newly heap-allocated.
*          therefore it has to be manually deallocated with free() after usage.
*
* @param[in] dict   the str_dictionary
*
* @return returns a string of all entries in dict on success,
*         or NULL on error
*/
attr_malloc
char* dict_getall(const str_dictionary* dict);


/**
* @brief inserts an entry with key and value into dict, or update 
*        the entry's value if entry with key already exists in dict
*
* @note if the dict's load factor reaches or exceeds
*       MAX_LOAD_FACTOR (default: 0.75), another call to
*       dict_insert will resize dict by RESIZE_FACTOR (default: 2)
*
* @param[in] dict   the str_dictionary
* @param[in] key    the key
* @param[in] value  the (new) value
*
* @return returns 0 on success, or -1 on error
*/
int32_t dict_insert(str_dictionary* dict, const char* key, const char* value);


/**
* @brief removes the entry with key from dict
*
* @param[in] dict   the str_dictionary
* @param[in] key    the key
*
* @return returns 0 on success, or -1 on error
*/
int32_t dict_remove(str_dictionary* dict, const char* key);


/**
* @brief deallocates the whole dict
*
* @param[in] dict the str_dictionary 
*
* @return returns 0 on success, or -1 on error
*/
int32_t dict_free(str_dictionary* dict);

#endif /* C_STR_DICTIONARY_H */