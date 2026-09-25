/**
* @file server.h
*
* @brief this module acts as a server engine (framework), which opens
*        raw tcp sockets, binded to an address specified by setting
*        the host and port through ceriousapi_setconf()
*
* @author 1810vincent (val <3)
*
* @date 09/2026
*/
#ifndef CERIOUSAPI_SERVER_H
#define CERIOUSAPI_SERVER_H


#include <stdint.h>
#include <pthread.h>


/**
* @brief sets the server's configurations (host, port, log level)
*
* @warning has to be called before starting the server with ceriousapi_startserver()
*
* @param[in] host       the host to be set
* @param[in] port       the port to be set
* @param[in] log_level  the log level to be set
*
* @return returns 0 on success, or -1 on error
*/
int32_t ceriousapi_setconf(const char* host, int32_t port, char* log_level);


#define MAX_ROUTES_AMOUNT 512
#define MAX_PATH_LEN 4096

typedef int32_t (*head_func)(int32_t, const char*);
typedef int32_t (*get_func)(int32_t, const char*);
typedef int32_t (*put_func)(int32_t, const char*, const char*);
typedef int32_t (*delete_func)(int32_t, const char*);

/**
* @brief sets the server's routing through the address path and HTTP method mapping
*
* @details if path is NULL or an empty string ("") it will get mapped to root ("/")
*
* @warning has to be called before starting the server with ceriousapi_startserver()
*
* @param[in] path           the address path which the functions should be mapped to - if NULL or an empty string ("") it will get mapped to root ("/")
* @param[in] head_func      mapped to the HTTP HEAD method
* @param[in] get_func       mapped to the HTTP GET method
* @param[in] put_func       mapped to the HTTP PUT method
* @param[in] delete_func    mapped to the HTTP DELETE method
*
* @return returns 0 on success, or -1 on error
*/
int32_t ceriousapi_setroute(const char* path, head_func, get_func, put_func, delete_func);


/**
* @brief starts the server
*
* @return returns the server main thread's pthread_t on success, or 0 on error
*/
pthread_t ceriousapi_startserver();


#define MAX_URL_LEN 4096
#define MAX_KEY_LEN ((MAX_URL_LEN / 2) - 16)
#define MAX_VALUE_LEN ((MAX_URL_LEN / 2) - 16)

#define MAX_RESPONSE_HEADER_LEN 1024
#define MAX_RESPONSE_BODY_LEN 4096

/**
* @brief sends an HTTP response to client
*
* @details constructs a complete HTTP/1.1 message with status line, default headers (Content-Length, Connection: close)
*          and an optional body, closes the client socket
*
* @warning closes the client socket
* 
* @param[in] client         the client socket file descriptor
* @param[in] status_code    the HTTP status code to send
* @param[in] body           the HTTP body to send
*
* @return returns 0 on success, or -1 on error
*/
int32_t respond(int client, int32_t status_code, const char* body);

#endif /* CERIOUSAPI_SERVER_H */