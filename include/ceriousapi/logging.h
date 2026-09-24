/**
* @file logging.h
*
* @brief this module provides all necessary logging functions for
*        the ceriousapi and extern modules. all logging functions
*        print the formatted log message to stdout / stderr
*        & return a specific value
*
* @details the message format in all functions is:
*          [{TYPE}] YYYY-MM-DD hh-mm-ss {timezone} | {modulename}: {message}
*
* @author 1810vincent (val <3)
*
* @date 09/2026s
*/
#ifndef CERIOUSAPI_LOGGING_H
#define CERIOUSAPI_LOGGING_H


#include <stdint.h>
#include <stdbool.h>


/**
* @brief thread-safe function to set the module's log level
*        from the level string
*
* @details available log levels:
*          INFO, WARNING, ERROR
*
* @param[in] level the log level to set
*/
void ceriousapi_log_set_level(char* level);


/*
* prints the log message with ERROR type
*/
void ceriousapi_log_server_error(const char* msg, const char* module);

/*
* prints the log message with ERROR type & returns -1
*/
int32_t ceriousapi_relog_server_error(const char* msg, const char* module);

/*
* prints the log message with ERROR type & returns 1
*/
int32_t ceriousapi_rpelog_server_error(const char* msg, const char* module);

/*
* prints the log message with ERROR type & returns -0
*/
uint32_t ceriousapi_rzlog_server_error(const char* msg, const char* module);

/*
* prints the log message with ERROR type & returns NULL
*/
void* ceriousapi_rnlog_server_error(const char* msg, const char* module);


/*
* prints the log message with INFO type
*/
void ceriousapi_log_server_info(const char* msg, const char* module);

/*
* prints the log message with INFO type & returns true
*/
bool ceriousapi_rtlog_server_info(const char* msg, const char* module);

/*
* prints the log message with INFO type & returns 0
*/
int32_t ceriousapi_rzlog_server_info(const char* msg, const char* module);


/*
* prints the log message with WARNING type
*/
void ceriousapi_log_server_warning(const char* msg, const char* module);

#endif /* CERIOUSAPI_LOGGING_H */