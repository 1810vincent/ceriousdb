#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <ceriousapi/ceriousapi.h>
#include "../dotenv/dotenv.h"

#define PORT_DEFAULT_STR "8000"

char DB_FILE[MAX_DBFILE_STRLEN] = DB_FILE_DEFAULT;
char LOG_LEVEL[MAX_LOGLEVEL_STRLEN] = LOG_LEVEL_DEFAULT;
char HOST[MAX_HOST_STRLEN] = HOST_DEFAULT;
int32_t PORT = PORT_DEFAULT;

static int32_t load_configs_from_env() {
    if (load_dotenv((int32_t)1) == (int32_t)-1)
        return ceriousapi_relog_server_error("could not load .env file", "CeriousDB/Config");
    snprintf(DB_FILE, sizeof(DB_FILE), "%s", get_env("CERIOUSDB_DB_FILE", DB_FILE_DEFAULT));
    snprintf(LOG_LEVEL, sizeof(LOG_LEVEL), "%s", get_env("LOG_LEVEL", LOG_LEVEL_DEFAULT));
    snprintf(HOST, sizeof(HOST), "%s", get_env("CERIOUSDB_HOST", HOST_DEFAULT));
    int32_t env_port = atoi(get_env("CERIOUSDB_PORT", PORT_DEFAULT_STR));
    if (!env_port)
        PORT = PORT_DEFAULT;
    else
        PORT = env_port;
    return (int32_t)0;
}

static int32_t load_configs_from_arg(int argc, char* argv[]) {
    struct option available_options[] = {
        {"host", required_argument, NULL, 'h'},
        {"port", required_argument, NULL, 'p'},
        {NULL, 0, NULL, 0}
    };
    const char shortopt[] = "h:p:";

    int user_option;
    int longind = 0;
    while ((user_option = getopt_long(argc, argv, shortopt, available_options, &longind)) != (int)-1) {
        switch (user_option) {
            case 'h':
                strncpy(HOST, optarg, sizeof(HOST) - 1);
                HOST[INET6_ADDRSTRLEN - 1] = '\0';
                break;
            case 'p':
                PORT = (int32_t)atoi(optarg);
                if ((PORT <= (int32_t)0) || (PORT > (int32_t)65535)) {
                    char error_msg[64];
                    snprintf(error_msg, sizeof(error_msg), "invalid port: %i", PORT);
                    return ceriousapi_relog_server_error(error_msg, "CeriousDB/Config");
                }
                else if ((PORT >= 1) && (PORT <= 1023)) {
                    char error_msg[128];
                    snprintf(error_msg, sizeof(error_msg), "using privileged port (needs sudo privileges) (1 - 1023): %i", PORT);
                    ceriousapi_log_server_warning(error_msg, "CeriousDB/Config");
                }
                break;
            default:
                return ceriousapi_relog_server_error("unsupported argument server option", "CeriousDB/Config");
        }
    }
    return (int32_t)0;
}

int32_t load_configs(int argc, char* argv[]) {
    if (load_configs_from_env() == (int32_t)-1)
        return ceriousapi_relog_server_error("could not load configs from env", "CeriousDB/Config");
    if (load_configs_from_arg(argc, argv) == (int32_t)-1)
        return ceriousapi_relog_server_error("could not load configs from arg", "CeriousDB/Config");
    return (int32_t)0;
}