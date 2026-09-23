#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200112L
#endif

#include "dotenv.h"


#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define SUCCESS (int32_t)0
#define FAILURE (int32_t)-1


#if !defined(_WIN32) && !defined(_WIN64)

    /* unix */
    #define env_set(name, value, overwrite) setenv((name), (value), (overwrite))


#else

    /* windows */
    #define env_set(name, value, overwrite) setenv_w((name), (value), (overwrite))

    /* 
    * custom setenv function for windows, since windows' _putenv_s()
    * natively does not support an overwrite specifier & overwrites on default
    * -> test and set
    */
    static int32_t setenv_w(const char* name, const char* value, bool overwrite) {
        if (overwrite)
            return (_putenv_s(name, value) == 0 ? SUCCESS : FAILURE);
        if (get_env(name, NULL))
            return SUCCESS;
        return (_putenv_s(name, value) == 0 ? SUCCESS : FAILURE); 
    }

#endif


/* in-place string trim quotes function -> returns str */
static char* trim_quote_pair(char* str) {
    if (!str)
        return NULL;

    size_t len = strlen(str);
    if (((str[0] == '"' && str[len - 1] == '"') || (str[0] == '\'' && str[len - 1] == '\'')) && (len >= 2UL))
    {
        str[len - 1] = '\0';
        str++;
    }

    return str;
}

/* in-place string trim function -> returns str */
static char* trim_spaces(char* str) {
    if (!str)
        return NULL;

    if (str[0] == '\0')
        return str;

    /* trim leading spaces */
    while(isspace((unsigned char)*str))
        str++;

    if (str[0] == '\0')
        return str;

    /* trim trailing spaces */
    char* end = str + strlen(str);
    end--;
    while(isspace((unsigned char)*end) && end > str)
        end--;
    *(end + 1) = '\0';

    return str;
}

static bool is_valid_name(const char* name) {
    if (!name || name[0] == '\0')
        return false;

    size_t len = strlen(name);
    if (((name[0] == '"' && name[len - 1] == '"') || (name[0] == '\'' && name[len - 1] == '\'')) && (len >= 3UL))
        return true;

    for (size_t i = (size_t)0UL; i < len; i++) {
        if (isspace((unsigned char)name[i])) {
            return false;
        }
    }
    return true;
}

static bool is_valid_value(const char* value) {
    if (!value)
        return false;

    if (value[0] == '\0')
        return true;

    size_t len = strlen(value);
    if (((value[0] == '"' && value[len - 1] == '"') || (value[0] == '\'' && value[len - 1] == '\'')) && (len >= 2UL))
        return true;

    for (size_t i = (size_t)0UL; i < len; i++) {
        if (isspace((unsigned char)value[i])) {
            return false;
        }
    }
    return true;
}

int32_t load_dotenv_f(const char* file, bool overwrite) {
    if (!file) {
        return FAILURE;
    }

    FILE* envf = fopen(file, "r");
    if (!envf) {
        return FAILURE;
    }

    int32_t ret = SUCCESS;
    char line[MAX_ENV_FILE_LINE_LEN];
    /* parse every line of the file (no breaks) */
    while (fgets(line, sizeof(line), envf)) {

        /* check if line exceeds MAX_ENV_FILE_LINE_LEN */
        size_t line_len = strlen(line);
        if ((line_len == (sizeof(line) - 1)) && (line[line_len - 1] != '\n') && (!feof(envf))) {
            /* trash the rest of the line for the next fgets -> shift the file's r/w-pointer to the end of the line */
            char c;
            while ((c = fgetc(envf)) != '\n' && c != EOF);
            ret = FAILURE;
            continue;
        }

        /* trim newline chars (unix: \n, windows: \r\n) */
        line[strcspn(line, "\r\n")] = '\0';

        char* trimmed_line = trim_spaces(line);

        /* skip empty lines and comment-only lines */
        if (trimmed_line[0] == '\0' || trimmed_line[0] == '#') {
            continue;
        }

        /* check for equals sign delimiter */
        char* delimiter = strchr(trimmed_line, '=');
        if (!delimiter) {
            ret = FAILURE;
            continue;
        }
        *delimiter = '\0';

        char* current_name = trim_quote_pair(trim_spaces(trimmed_line));
        char* current_value = trim_spaces(delimiter + 1);

        if (!is_valid_name(current_name)) {
            ret = FAILURE;
            continue;
        }

        /* handle inline-comments (stored in value until here) */
        if (current_value[0] != '"' && current_value[0] != '\'') {
            char* comment_delimiter = strchr(current_value, '#');
            if (comment_delimiter) {
                *comment_delimiter = '\0';
                current_value = trim_spaces(current_value);
            }
        }

        if (!is_valid_value(current_value)) {
            ret = FAILURE;
            continue;
        }

        current_value = trim_quote_pair(current_value);

        if (env_set(current_name, current_value, overwrite) != 0) {
            ret = FAILURE;
        }
    }

    fclose(envf);
    return ret;
}

int32_t load_dotenv(bool overwrite) {
    return load_dotenv_f(".env", overwrite);
}


const char* get_env(const char* name, const char* fallback) {
    if (!name)
        return fallback;

    char* value = getenv(name);
    if (!value)
        return fallback;
    return (const char*)value;
}