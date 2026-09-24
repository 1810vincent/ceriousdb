#define DBNAME "CeriousDB(v0.1.0)"

#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <ceriousapi/ceriousapi.h>

#include "cache.h"
#include "config.h"
#include "interface.h"

cache* db_cache;


#define DB_PATH "/db"

int32_t head(int32_t client, const char* key) {
    const char* value = cache_get(db_cache, key, NULL);
    if (value == NULL)
        return respond(client, 404, NULL, NOSENDBODY);
    return respond(client, 200, value, NOSENDBODY);
}

int32_t get(int32_t client, const char* key) {
    const char* value = cache_get(db_cache, key, NULL);
    if (value == NULL) {
        char body[22 + MAX_KEY_LEN + 1];
        snprintf(body, sizeof(body), "No value set for key %s", key);
        return respond(client, 404, body, SENDBODY);
    }
    return respond(client, 200, value, SENDBODY);
}

int32_t put(int32_t client, const char* key, const char* value) {
    if (cache_insert(db_cache, key, value) == -1)
        return respond(client, 500, "Could not insert entry", SENDBODY);
    if (cache_flush(db_cache) == -1)    //TODO: make flushing a background task
        return respond(client, 500, "Database file could not be opened and loaded", SENDBODY);
    return respond(client, 200, value, SENDBODY);
}

int32_t handle_delete(int32_t client, const char* key) {
    if (cache_remove(db_cache, key) == (int32_t)-1) {
        char body[22 + MAX_KEY_LEN + 1];
        snprintf(body, sizeof(body), "No entry for key %s", key);
        return respond(client, 404, body, SENDBODY);
    }
    if (cache_flush(db_cache) == -1)    //TODO: make flushing a background task
        return respond(client, 500, "Database file could not be opened and loaded", SENDBODY);
    return respond(client, 200, "Entry successfully deleted", SENDBODY);
}


#define DB_ALL_PATH "/db/all"

int32_t get_all(int32_t client, __attribute__((unused)) const char* key) {
    const char* all = cache_getall(db_cache);
    if (all == NULL)
        return respond(client, 503, "Internal Server Error", SENDBODY);
    return respond(client, 200, all, SENDBODY);
}

/* TODO: implement -> support multiple keys for get bulk
*
#define DB_BULK_PATH "/db/bulk"

int32_t get_bulk(int32_t client, const char* keys[]);
*/

#define DB_COUNT_PATH "/db/count"

int32_t get_count(int32_t client, __attribute__((unused)) const char* key) {
    int32_t count = cache_len(db_cache);
    if (count == -1)
        return respond(client, 503, "Internal Server Error", SENDBODY);
    char body[128];
    snprintf(body, sizeof(body), "DB contains %i entries", count);
    return respond(client, 200, body, SENDBODY);
}


#define HEALTH_PATH "/health"

int32_t get_health(int32_t client, __attribute__((unused)) const char* key) {
    if (db_cache->db == NULL)
        return respond(client, 503, "Service unavailable", SENDBODY);
    return respond(client, 200, "{\"status\": \"ok\"}", SENDBODY);
}


int32_t run_app() {

    db_cache = cache_init();
    if (!db_cache)
        return ceriousapi_rpelog_server_error("cache could not be initialized", DBNAME);
    if (cache_load(db_cache, DB_FILE) == (int32_t)-1)
        return ceriousapi_rpelog_server_error("cache could not load file", DBNAME);

    if ((ceriousapi_setroute(DB_PATH, head, get, put, handle_delete) == (int32_t)-1)
    || (ceriousapi_setroute(DB_ALL_PATH, NULL, get_all, NULL, NULL) == (int32_t)-1)
    || (ceriousapi_setroute(DB_COUNT_PATH, NULL, get_count, NULL, NULL) == (int32_t)-1)
    || (ceriousapi_setroute(HEALTH_PATH, NULL, get_health, NULL, NULL) == (int32_t)-1))
        return ceriousapi_rpelog_server_error("could not set server route", DBNAME);

    pthread_t app = ceriousapi_startserver();
    if (app == (pthread_t)0)
        return ceriousapi_rpelog_server_error("could not start server", DBNAME);

    if (server_interface() == (int32_t)-1)
        return ceriousapi_rpelog_server_error("error in user-server interface", DBNAME);
    return ceriousapi_rzlog_server_info("server stopped", DBNAME);
}

int main(int argc, char* argv[]) {
    if (load_configs(argc, argv) == (int32_t)-1)
        exit(ceriousapi_rpelog_server_error("could not load configs", DBNAME));
    if (ceriousapi_setopt(HOST, PORT, LOG_LEVEL) == (int32_t)-1)
        exit(ceriousapi_rpelog_server_error("could not set server options", DBNAME));
    exit((int)run_app());
}