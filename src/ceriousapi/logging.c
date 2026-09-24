#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200112L
#endif

#include <ceriousapi/ceriousapi.h>


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#if !defined(_WIN32) && !defined(_WIN64) /* unix */

    #include <pthread.h>


    static pthread_mutex_t setlevel_mutex = PTHREAD_MUTEX_INITIALIZER;
    
    #define lock_set()   pthread_mutex_lock(&setlevel_mutex)
    #define unlock_set() pthread_mutex_unlock(&setlevel_mutex)

#else /* windows */

    #include <windows.h>


    static SRWLOCK setlevel_mutex = SRWLOCK_INIT;

    #define lock_set()   AcquireSRWLockExclusive(&setlevel_mutex)
    #define unlock_set() ReleaseSRWLockExclusive(&setlevel_mutex)

#endif


typedef enum { ERROR, WARNING, INFO } log_type;


static log_type log_level = INFO;


static inline char* stringof_logtype(log_type type) {
    switch (type) {
        case INFO:      return "INFO";
        case WARNING:   return "WARNING";
        case ERROR:     return "ERROR";
        default:        return "UNKNOWN";
    }
}

static inline void to_upper(char* str) {
    for (size_t i = (size_t)0UL; str[i] != '\0'; i++) {
        if ((str[i] >= 'a') && (str[i] <= 'z'))
            str[i] -= ('v' - 'V');
    }
    return;
}

static log_type get_log_level_print_error(char* level_string) {
    if (!level_string)
        return INFO;
    char error_msg[1024];
    snprintf(error_msg, sizeof(error_msg), "could not correctly get log level - invalid log level: %s, fallback to default log level: %s", level_string, stringof_logtype(INFO));
    ceriousapi_log_server_error(error_msg, CERIOUSAPINAME);
    return INFO;
}

static log_type get_log_level_from_string(char* level_string) {
    if (!level_string)
        return get_log_level_print_error("...");

    char level_string_copy[128];
    int32_t written = (int32_t)snprintf(level_string_copy, sizeof(level_string_copy), "%s", level_string);
    if (written < 0) {
        return get_log_level_print_error(level_string);
    }

    to_upper(level_string_copy);

    if (strcmp(level_string_copy, "INFO") == 0)
        return INFO;
    if (strcmp(level_string_copy, "WARNING") == 0)
        return WARNING;
    if (strcmp(level_string_copy, "ERROR") == 0)
        return ERROR;

    return get_log_level_print_error(level_string);
}

void ceriousapi_log_set_level(char* level) {
    lock_set();
    log_level = get_log_level_from_string(level);
    unlock_set();
    return;
}


#define TIMESTAMP_BUFFER_SIZE 256
#define TIMESTAMP_FALLBACK "..."

static int32_t get_timestamp_fstring(char* buffer) {
    time_t rawtime;
    if (time(&rawtime) == (time_t)-1) {
        strncpy(buffer, TIMESTAMP_FALLBACK, TIMESTAMP_BUFFER_SIZE);
        return (int32_t)-1;
    }

    struct tm timeinfo;

#if !defined(_WIN32) && !defined(_WIN64) /* unix */
    if (!localtime_r(&rawtime, &timeinfo)) {
#else /* windows */
    if (localtime_s(&timeinfo, &rawtime) != 0) {
#endif
        strncpy(buffer, TIMESTAMP_FALLBACK, TIMESTAMP_BUFFER_SIZE);
        return (int32_t)-1;
    }

    if (strftime(buffer, TIMESTAMP_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S %Z", &timeinfo) == 0) {
        snprintf(buffer, TIMESTAMP_BUFFER_SIZE, "...");
        return (int32_t)-1;
    }
    return (int32_t)0;
}

static inline const char* color_stringof_type(log_type type) {
    switch (type) {
        case INFO:      return "\33[34mINFO\33[0m";
        case WARNING:   return "\33[33mWARNING\33[0m";
        case ERROR:     return "\33[31mERROR\33[0m";
        default:        return "\33[36mUNKNOWN\33[0m";
    }
}

static void log_server_message(log_type type, const char* msg, const char* module) {
    if (type > log_level)
        return;

    char timestamp[TIMESTAMP_BUFFER_SIZE];
    if (get_timestamp_fstring(timestamp) == (int32_t)-1)
        snprintf(timestamp, TIMESTAMP_BUFFER_SIZE, "...");

    FILE* fp = ((type == WARNING) || (type == ERROR)) ? stderr : stdout;

    if (msg && module)
        fprintf(fp, "\r\33[2K[%s] %s | server/%s: %s\n", color_stringof_type(type), timestamp, module, msg);
    else if (msg && !module)
        fprintf(fp, "\r\33[2K[%s] %s | server: %s\n", color_stringof_type(type), timestamp, msg);
    else if (!msg && module)
        fprintf(fp, "\r\33[2K[%s] %s | server/%s\n", color_stringof_type(type), timestamp, module);
    else
        fprintf(fp, "\r\33[2K[%s] %s | server\n", color_stringof_type(type), timestamp);

    return;
}


void ceriousapi_log_server_error(const char* msg, const char* module) {
    log_server_message(ERROR, msg, module);
    return;
}

int32_t ceriousapi_relog_server_error(const char* msg, const char* module) {
    log_server_message(ERROR, msg, module);
    return (int32_t)-1;
}

int32_t ceriousapi_rpelog_server_error(const char* msg, const char* module) {
    log_server_message(ERROR, msg, module);
    return (int32_t)1;
}

uint32_t ceriousapi_rzlog_server_error(const char* msg, const char* module) {
    log_server_message(ERROR, msg, module);
    return (uint32_t)0U;
}

void* ceriousapi_rnlog_server_error(const char* msg, const char* module) {
    log_server_message(ERROR, msg, module);
    return NULL;
}


void ceriousapi_log_server_info(const char* msg, const char* module) {
    log_server_message(INFO, msg, module);
    return;
}

bool ceriousapi_rtlog_server_info(const char* msg, const char* module) {
    log_server_message(INFO, msg, module);
    return true;
}

int32_t ceriousapi_rzlog_server_info(const char* msg, const char* module) {
    log_server_message(INFO, msg, module);
    return (int32_t)0;
}


void ceriousapi_log_server_warning(const char* msg, const char* module) {
    log_server_message(WARNING, msg, module);
    return;
}