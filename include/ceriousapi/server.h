#ifndef CERIOUSAPI_SERVER_H
#define CERIOUSAPI_SERVER_H


#include <pthread.h>
#include <stdint.h>


#define MAX_PATH_LEN 4096
#define MAX_KEY_LEN ((MAX_PATH_LEN / 2) - 16)
#define MAX_VALUE_LEN ((MAX_PATH_LEN / 2) - 16)

#define MAX_RESPONSE_HEADER_LEN 1024
#define MAX_RESPONSE_LEN 4096


typedef enum {NOSENDBODY, SENDBODY} send_body;


/**
* @brief sends an http response to the client
* @details constructs a complete HTTP/1.1 message with status line, default headers (Content-Length, Connection: close)
*          and an optional body, closes the client socket
*
* @warning closes the client socket
* 
* @param[in] client         the client socket file descriptor
* @param[in] status_code    the HTTP status code to send
* @param[in] body           the HTTP body to send (can only be NULL if send_body is set to NOSENDBODY)
* @param[in] send_body      flag specifying whether to send the body (SENDBODY / NOSENDBODY)
*
* @return int32_t           returns 0 on success, or -1 on error
*/
int32_t respond(int32_t client, int32_t status_code, const char* body, send_body send_body);


/**
* @brief sets the server's options to user's optional startup argument options (host, port)
* @details available options:
*               host: "-h" [hostname] / "--host" [hostname]
*               port: "-p" [portnumber] / "--port" [portnumber]
*
* @warning has to be called with argc and argv from main(int argc, char* argv[]) before starting the server with ceriousapi_startserver()
*
* @param[in] argc   argument count from executing application
* @param[in] argv   arguments from executing application (host, port)
*
* @return int32_t   returns 0 on success, -1 on error
*/
int32_t ceriousapi_setopt(const char* host, int32_t port, const char* log_level);


#define MAX_ROUTES_AMOUNT 512


typedef int32_t (*head_func)(int32_t, const char*);
typedef int32_t (*get_func)(int32_t, const char*);
typedef int32_t (*put_func)(int32_t, const char*, const char*);
typedef int32_t (*delete_func)(int32_t, const char*);


/**
* @brief sets the server's routing through the address path and http method mapping
*
* @warning has to be called before starting the server with ceriousapi_startserver()
* @warning if path is NULL or an empty string ("") it will get mapped to root ("/")
*
* @param[in] path           the address path which the functions should be mapped to, if path is NULL or an empty string ("") it will get mapped to root ("/")
* @param[in] head_func      mapped to the http HEAD method
* @param[in] get_func       mapped to the http GET method
* @param[in] put_func       mapped to the http PUT method
* @param[in] delete_func    mapped to the http DELETE method
*
* @return returns 0 on success, -1 on error
*/
int32_t ceriousapi_setroute(const char* path, head_func, get_func, put_func, delete_func);


/**
* @brief starts server thread
*
* @return returns the server thread's pthread_t on success, 0 on error
*/
pthread_t ceriousapi_startserver();

#endif /* CERIOUSAPI_SERVER_H */