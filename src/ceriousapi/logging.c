#include <ceriousapi/ceriousapi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


typedef enum { ERROR, WARNING, INFO } log_type;

static log_type log_level = INFO;

static inline const char* stringoftype(log_type type) {
    switch (type) {
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

#define MAX_LOGLEVELERRORMSG_LEN 1024

static inline log_type get_loglevel_fromstring(const char* level_string) {
    if (strcmp(level_string, stringoftype(INFO)) == 0)
        return INFO;
    if (strcmp(level_string, stringoftype(WARNING)) == 0)
        return WARNING;
    if (strcmp(level_string, stringoftype(ERROR)) == 0)
        return ERROR;
    char error_msg[MAX_LOGLEVELERRORMSG_LEN];
    snprintf(error_msg, sizeof(error_msg), "could not correctly get log level from .env, invalid log level: %s, fallback to level: %s", level_string, "INFO");
    ceriousapi_log_server_error(error_msg, SERVERNAME);
    return INFO;
}

void ceriousapi_set_log_level(const char* level) {
    log_level = get_loglevel_fromstring(level);
}


#define MAX_TIMESTAMP_STRLEN 128
#define TIMESTAMP_FALLBACK ""

static char* get_timestamp_fstring() {
    char* timestamp_fstring = (char*)malloc(MAX_TIMESTAMP_STRLEN * sizeof(*timestamp_fstring));
    if (!timestamp_fstring)
        return TIMESTAMP_FALLBACK;

    time_t rawtime;
    time(&rawtime);

    struct tm* timeinfo = localtime(&rawtime);
    if (!timeinfo) {
        free(timestamp_fstring);
        return TIMESTAMP_FALLBACK;
    }

    if (strftime(timestamp_fstring, MAX_TIMESTAMP_STRLEN, "%Y-%m-%d %H:%M:%S %Z", timeinfo) == 0) {
        free(timestamp_fstring);
        return TIMESTAMP_FALLBACK;
    }
    return timestamp_fstring;
}

static inline const char* cstringoftype(log_type type) {
    switch (type) {
        case INFO: return "\33[34mINFO\33[0m";
        case WARNING: return "\33[33mWARNING\33[0m";
        case ERROR: return "\33[31mERROR\33[0m";
        default: return "\33[33mUNKNOWN\33[0m";
    }
}

/* does not interrupt the interface's output when server is running */
static void log_server_message(log_type type, const char* msg, const char* module, char* timestamp) {
    if (type > log_level)
        return;

    if (!timestamp)
        timestamp = TIMESTAMP_FALLBACK;

    FILE* fp = ((type == WARNING) || (type == ERROR)) ? stderr : stdout;

    if (msg && module)
        fprintf(fp, "\r\33[2K[%s] %s | server/%s: %s\n", cstringoftype(type), timestamp, module, msg);
    else if (msg && !module)
        fprintf(fp, "\r\33[2K[%s] %s | server: %s\n", cstringoftype(type), timestamp, msg);
    else if (!msg && module)
        fprintf(fp, "\r\33[2K[%s] %s | server/%s\n", cstringoftype(type), timestamp, module);
    else
        fprintf(fp, "\r\33[2K[%s] %s | server\n", cstringoftype(type), timestamp);

    return;
}


void ceriousapi_log_server_error(const char* msg, const char* module) {
    char* timestamp = get_timestamp_fstring();
    log_server_message(ERROR, msg, module, timestamp);
    if (!(strcmp(timestamp, TIMESTAMP_FALLBACK) == 0))
        free(timestamp);
    return;
}

int32_t ceriousapi_relog_server_error(const char* msg, const char* module) {
    ceriousapi_log_server_error(msg, module);
    return (int32_t)-1;
}

int32_t ceriousapi_rpelog_server_error(const char* msg, const char* module) {
    ceriousapi_log_server_error(msg, module);
    return (int32_t)-1;
}

uint32_t ceriousapi_rzlog_server_error(const char* msg, const char* module) {
    ceriousapi_log_server_error(msg, module);
    return (uint32_t)0;
}

void* ceriousapi_rnlog_server_error(const char* msg, const char* module) {
    ceriousapi_log_server_error(msg, module);
    return NULL;
}


void ceriousapi_log_server_info(const char* msg, const char* module) {
    char* timestamp = get_timestamp_fstring();
    log_server_message(INFO, msg, module, timestamp);
    if (!(strcmp(timestamp, TIMESTAMP_FALLBACK) == 0))
        free(timestamp);
    return;
}

bool ceriousapi_rtlog_server_info(const char* msg, const char* module) {
    ceriousapi_log_server_info(msg, module);
    return true;
}

int32_t ceriousapi_rzlog_server_info(const char* msg, const char* module) {
    ceriousapi_log_server_info(msg, module);
    return (int32_t)0;
}


void ceriousapi_log_server_warning(const char* msg, const char* module) {
    char* timestamp = get_timestamp_fstring();
    log_server_message(WARNING, msg, module, timestamp);
    if (!(strcmp(timestamp, TIMESTAMP_FALLBACK) == 0))
        free(timestamp);
    return;
}