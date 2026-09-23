#ifndef CERIOUSAPI_LOGGING_H
#define CERIOUSAPI_LOGGING_H


#include <stdint.h>
#include <stdbool.h>


/* available log levels in order: INFO, WARNING, ERROR */
void ceriousapi_set_log_level(const char* level);


/* does not interrupt the interface's output */
void ceriousapi_log_server_error(const char* msg, const char* module);

/* print_server_error and return -1 (error code) */
int32_t ceriousapi_relog_server_error(const char* msg, const char* module);

/* print_server_error and return 1 (error code) */
int32_t ceriousapi_rpelog_server_error(const char* msg, const char* module);

/* print_server_error and return 0 */
uint32_t ceriousapi_rzlog_server_error(const char* msg, const char* module);

/* print_server_error and return NULL */
void* ceriousapi_rnlog_server_error(const char* msg, const char* module);


void ceriousapi_log_server_info(const char* msg, const char* module);

/* returns true */
bool ceriousapi_rtlog_server_info(const char* msg, const char* module);

/* returns 0 */
int32_t ceriousapi_rzlog_server_info(const char* msg, const char* module);


void ceriousapi_log_server_warning(const char* msg, const char* module);

#endif /* CERIOUSAPI_LOGGING_H */