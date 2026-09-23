#include <ceriousapi/ceriousapi.h>
#include <ceriousapi/logging.h>

#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/* configs (default values) */
static char host[INET6_ADDRSTRLEN] = "127.0.0.1\0";
static int32_t port = (int32_t)8000;


static int32_t server = 0;


typedef struct {
    char path[MAX_PATH_LEN];
    head_func head;
    get_func get;
    put_func put;
    delete_func delete;
} route;

static route routes[MAX_ROUTES_AMOUNT];
static int32_t route_count = 0;


#define HTTPM(METHOD) #METHOD
#define MAX_METHOD_STRLEN 7


#define HTTP_VERSION "HTTP/1.1"
#define MAX_STATUS_LEN 64
#define HEAD_BODY_DIVIDER "\r\n\r\n"

static void get_statusstring(char* status_string, int32_t status_code, size_t max_len);

int32_t respond(int client, int32_t status_code, const char* body, send_body send_body) {
    char status[MAX_STATUS_LEN];
    get_statusstring(status, status_code, sizeof(status));

    size_t body_len = (body && send_body) ? strlen(body) : (size_t)0L;

    char header_buffer[MAX_RESPONSE_HEADER_LEN];
    int header_len = snprintf(header_buffer, sizeof(header_buffer),
        "%s %s\r\n"
        "Server: %s\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "%s",
        HTTP_VERSION, status, SERVERNAME, body_len, HEAD_BODY_DIVIDER);

    if (header_len < 0 || (size_t)header_len >= sizeof(header_buffer)) {
        close(client);
        return ceriousapi_relog_server_error("could not load http header into buffer - intended http header is too big for dedicated header buffer", SERVERNAME);
    }

    if (write(client, header_buffer, header_len) < (int)0) {
        close(client);
        return ceriousapi_relog_server_error("could not write http header buffer to client socket", SERVERNAME);
    }

    if (body && send_body && body_len > 0) {
        if (write(client, body, body_len) < 0) {
            close(client);
            return ceriousapi_relog_server_error("could not write http body to client socket", SERVERNAME);
        }
    }

    close(client);
    return (int32_t)0;
}


int32_t ceriousapi_setopt(const char* new_host, int32_t new_port, const char* log_level) {
    if (!new_host || !log_level)
        return ceriousapi_relog_server_error("could not set ceriousapi server options", SERVERNAME);
    strncpy(host, new_host, sizeof(host) - 1);
    port = new_port;
    ceriousapi_set_log_level(log_level);
    return (int32_t)0;
}


int32_t ceriousapi_setroute(const char* path, head_func head_f, get_func get_f, put_func put_f, delete_func delete_f) {
    if (route_count >= MAX_ROUTES_AMOUNT)
        return ceriousapi_relog_server_error("maximum of programmable routes reached", SERVERNAME);

    if (!path || (strcmp(path, "") == 0))
        path = "/";

    strncpy(routes[route_count].path, path, MAX_PATH_LEN - 1);
    routes[route_count].path[MAX_PATH_LEN - 1] = '\0';
    
    routes[route_count].head = head_f;
    routes[route_count].get = get_f;
    routes[route_count].put = put_f;
    routes[route_count].delete = delete_f;

    route_count++;
    return (int32_t)0;
}


static int32_t parse_requestline(int32_t client, char* path, char* method, char* key, char* value) {

    char request_line[MAX_PATH_LEN + MAX_METHOD_STRLEN + MAX_KEY_LEN + MAX_VALUE_LEN + 1];
    ssize_t bytes_read = read(client, request_line, sizeof(request_line) - 1);
    if (bytes_read <= 0) {
        return ceriousapi_relog_server_error("could not read from client socket", SERVERNAME);
    }
    request_line[bytes_read] = '\0';

    /* isolate request line */
    char* newline = strpbrk(request_line, "\r\n");
    if (newline) *newline = '\0';

    /* parse method */
    char* method_token = strtok(request_line, " ");
    if (!method_token)
        return ceriousapi_relog_server_error("no method token", SERVERNAME);
    else if (strcmp(method_token, HTTPM(HEAD)) == 0)
        snprintf(method, MAX_METHOD_STRLEN, "%s", HTTPM(HEAD));
    else if (strcmp(method_token, HTTPM(GET)) == 0)
        snprintf(method, MAX_METHOD_STRLEN, "%s", HTTPM(GET));
    else if (strcmp(method_token, HTTPM(PUT)) == 0)
        snprintf(method, MAX_METHOD_STRLEN, "%s", HTTPM(PUT));
    else if (strcmp(method_token, HTTPM(DELETE)) == 0)
        snprintf(method, MAX_METHOD_STRLEN, "%s", HTTPM(DELETE));
    else
        ceriousapi_log_server_error("request has unsupported http method", SERVERNAME);

    /* parse path */
    char* path_token = strtok(NULL, " ");
    if (path_token)
        snprintf(path, MAX_PATH_LEN, "%s", path_token);
    else
        snprintf(path, MAX_PATH_LEN, "/");

    (void)key;
    (void)value;

    return (int32_t)0;
}

static int32_t execute_function(int32_t client, char* path, char* method, char* key, char* value) {
    if (client == 0)
        return ceriousapi_relog_server_error("could not execute function - client is not given", SERVERNAME);
    if (!path)
        return ceriousapi_relog_server_error("could not execute function - path is not given", SERVERNAME);
    if (!method)
        return ceriousapi_relog_server_error("could not execute function - method is not given", SERVERNAME);
    
    for (int32_t i = 0; i < route_count; i++) {
        if (strcmp(path, routes[i].path) == 0) {
            if (strcmp(method, HTTPM(HEAD)) == 0) {
                if (!routes[i].head)
                    return ceriousapi_relog_server_error("server route with unmapped http method got called", SERVERNAME);
                return routes[i].head(client, key);
            }
            else if (strcmp(method, HTTPM(GET)) == 0) {
                if (!routes[i].get)
                    return ceriousapi_relog_server_error("server route with unmapped http method got called", SERVERNAME);
                return routes[i].get(client, key);
            }
            else if (strcmp(method, HTTPM(PUT)) == 0) {
                if (!routes[i].put)
                    return ceriousapi_relog_server_error("server route with unmapped http method got called", SERVERNAME);
                return routes[i].put(client, key, value);
            }
            else if (strcmp(method, HTTPM(DELETE)) == 0) {
                if (!routes[i].delete)
                    return ceriousapi_relog_server_error("server route with unmapped http method got called", SERVERNAME);
                return routes[i].delete(client, key);
            }
            else
                return ceriousapi_relog_server_error("server route with unmapped http method got called", SERVERNAME);
        }
    }
    return (int32_t)-1;
}

static void* process_single_request(void* arg) {
    if (!arg)
        return ceriousapi_rnlog_server_error("worker thread could not receive arg", SERVERNAME);
    int32_t client = *(int32_t*)arg;
    free(arg);

    char path[MAX_PATH_LEN] = "";
    char method[MAX_METHOD_STRLEN] = "";
    char key[MAX_KEY_LEN] = "";
    char value[MAX_VALUE_LEN] = "";

    if (parse_requestline(client, path, method, key, value) == -1) {
        close(client);
        return ceriousapi_rnlog_server_error("could not parse request line", SERVERNAME);
    }
    
    if (execute_function(client, path, method, key, value) == -1) {
        close(client);
        return ceriousapi_rnlog_server_error("error while executing method", SERVERNAME);
    }

    return NULL;
}

#define MAX_STARTUPMSG_LEN 256

static void* server_loop(__attribute__((unused)) void* arg) {
    char startup_msg[MAX_STARTUPMSG_LEN];
    snprintf(startup_msg, MAX_STARTUPMSG_LEN, "server running and accessible at [ http://%s:%i ]", host, port);
    ceriousapi_log_server_info(startup_msg, SERVERNAME);

    int32_t client;
    while ((client = accept(server, 0, 0)) >= 0) {
        ceriousapi_log_server_info("request received", SERVERNAME);
        
        int32_t* clientptr = (int32_t*)malloc(sizeof(*clientptr));
        if (!clientptr) {
            ceriousapi_log_server_error("could not allocate memory for server loop thread arg", SERVERNAME);
            close(client);
            continue;
        }
        *clientptr = client;

        pthread_t worker_thread;
        if (pthread_create(&worker_thread, NULL, process_single_request, clientptr) != 0) {
            ceriousapi_log_server_error("could not create worker thread", SERVERNAME);
            free(clientptr);
            close(client);
            continue;
        }

        if (pthread_detach(worker_thread) != 0) {
            ceriousapi_log_server_error("could not detach worker thread", SERVERNAME);
            pthread_join(worker_thread, NULL);
            continue;
        }
    }
    return NULL;
}

pthread_t ceriousapi_startserver() {
    if ((server = (int32_t)socket(AF_INET, SOCK_STREAM, 0)) == (int32_t)-1)
        return ceriousapi_rzlog_server_error("could not create the server's socket ", SERVERNAME);

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(host),
        .sin_port = htons(port)
    };

    if (bind(server, (struct sockaddr*)&address, sizeof(address)) == (int)-1)
        return (pthread_t)ceriousapi_rzlog_server_error("could not bind the server to the address", SERVERNAME);
    
    if (listen(server, SOMAXCONN) == (int)-1)
        return (pthread_t)ceriousapi_rzlog_server_error("could not prepare the server to accept incoming connections", SERVERNAME);

    pthread_t server_thread;
    if (pthread_create(&server_thread, NULL, server_loop, NULL) != 0)
        return (pthread_t)ceriousapi_rzlog_server_error("could not create server loop thread", SERVERNAME);

    if (pthread_detach(server_thread) != 0)
        return (pthread_t)ceriousapi_rzlog_server_error("could not detach server loop thread", SERVERNAME);

    return server_thread;
}


static void get_statusstring(char* status_string, int32_t status_code, size_t max_len) {
    switch (status_code) {
        case 100: snprintf(status_string, max_len, "%i Continue", status_code); return;
        case 101: snprintf(status_string, max_len, "%i Switching Protocols", status_code); return;
        case 103: snprintf(status_string, max_len, "%i Early Hints", status_code); return;
        case 200: snprintf(status_string, max_len, "%i OK", status_code); return;
        case 201: snprintf(status_string, max_len, "%i Created", status_code); return;
        case 202: snprintf(status_string, max_len, "%i Accepted", status_code); return;
        case 203: snprintf(status_string, max_len, "%i Non-Authoritative Information", status_code); return;
        case 204: snprintf(status_string, max_len, "%i No Content", status_code); return;
        case 205: snprintf(status_string, max_len, "%i Reset Content", status_code); return;
        case 206: snprintf(status_string, max_len, "%i Partial Content", status_code); return;
        case 207: snprintf(status_string, max_len, "%i Multi-Status", status_code); return;
        case 208: snprintf(status_string, max_len, "%i Already Reported", status_code); return;
        case 226: snprintf(status_string, max_len, "%i IM Used", status_code); return;
        case 300: snprintf(status_string, max_len, "%i Multiple Choices", status_code); return;
        case 301: snprintf(status_string, max_len, "%i Moved Permanently", status_code); return;
        case 302: snprintf(status_string, max_len, "%i Found", status_code); return;
        case 303: snprintf(status_string, max_len, "%i See Other", status_code); return;
        case 304: snprintf(status_string, max_len, "%i Not Modified", status_code); return;
        case 306: snprintf(status_string, max_len, "%i Switch Proxy", status_code); return;
        case 307: snprintf(status_string, max_len, "%i Temporary Redirect", status_code); return;
        case 308: snprintf(status_string, max_len, "%i Permanent Redirect", status_code); return;
        case 400: snprintf(status_string, max_len, "%i Bad Request", status_code); return;
        case 401: snprintf(status_string, max_len, "%i Unauthorized", status_code); return;
        case 402: snprintf(status_string, max_len, "%i Payment Required", status_code); return;
        case 403: snprintf(status_string, max_len, "%i Forbidden", status_code); return;
        case 404: snprintf(status_string, max_len, "%i Not Found", status_code); return;
        case 405: snprintf(status_string, max_len, "%i Method Not Allowed", status_code); return;
        case 406: snprintf(status_string, max_len, "%i Not Acceptable", status_code); return;
        case 407: snprintf(status_string, max_len, "%i Proxy Authentication Required", status_code); return;
        case 408: snprintf(status_string, max_len, "%i Request Timeout", status_code); return;
        case 409: snprintf(status_string, max_len, "%i Conflict", status_code); return;
        case 410: snprintf(status_string, max_len, "%i Gone", status_code); return;
        case 411: snprintf(status_string, max_len, "%i Length Required", status_code); return;
        case 412: snprintf(status_string, max_len, "%i Precondition Failed", status_code); return;
        case 413: snprintf(status_string, max_len, "%i Content Too Large", status_code); return;
        case 414: snprintf(status_string, max_len, "%i URI Too Long", status_code); return;
        case 415: snprintf(status_string, max_len, "%i Unsupported Media Type", status_code); return;
        case 416: snprintf(status_string, max_len, "%i Range Not Satisfiable", status_code); return;
        case 417: snprintf(status_string, max_len, "%i Expectation Failed", status_code); return;
        case 418: snprintf(status_string, max_len, "%i I'm a teapot", status_code); return;
        case 421: snprintf(status_string, max_len, "%i Misdirected Request", status_code); return;
        case 422: snprintf(status_string, max_len, "%i Unprocessable Content", status_code); return;
        case 423: snprintf(status_string, max_len, "%i Locked", status_code); return;
        case 424: snprintf(status_string, max_len, "%i Failed Dependency", status_code); return;
        case 425: snprintf(status_string, max_len, "%i Too Early", status_code); return;
        case 426: snprintf(status_string, max_len, "%i Update Required", status_code); return;
        case 428: snprintf(status_string, max_len, "%i Precondition Required", status_code); return;
        case 429: snprintf(status_string, max_len, "%i Too Many Requests", status_code); return;
        case 431: snprintf(status_string, max_len, "%i Request Header Fields Too Large", status_code); return;
        case 451: snprintf(status_string, max_len, "%i Unavailable For Legal Reasons", status_code); return;
        case 500: snprintf(status_string, max_len, "%i Internal Server Error", status_code); return;
        case 502: snprintf(status_string, max_len, "%i Bad Gateway", status_code); return;
        case 503: snprintf(status_string, max_len, "%i Service Unavailable", status_code); return;
        case 504: snprintf(status_string, max_len, "%i Gateway Timeout", status_code); return;
        case 505: snprintf(status_string, max_len, "%i HTTP Version Not Supported", status_code); return;
        case 506: snprintf(status_string, max_len, "%i Variant Also Negotiates", status_code); return;
        case 507: snprintf(status_string, max_len, "%i Insufficient Storage", status_code); return;
        case 508: snprintf(status_string, max_len, "%i Loop Detected", status_code); return;
        case 510: snprintf(status_string, max_len, "%i Not Extended", status_code); return;
        case 511: snprintf(status_string, max_len, "%i Network Authentication Required", status_code); return;
        default:
            snprintf(status_string, max_len, "500 Internal Server Error");
            return ceriousapi_log_server_warning("invalid status code passed by application, falling back to 500", SERVERNAME);
    }
}