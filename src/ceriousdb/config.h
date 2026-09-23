#ifndef CERIOUSDB_CONFIG_H
#define CERIOUSDB_CONFIG_H

#include <stdint.h>
#include <netinet/in.h>

#define MAX_DBFILE_STRLEN 256
extern char DB_FILE[MAX_DBFILE_STRLEN];
#define DB_FILE_DEFAULT ".cdb"

#define MAX_LOGLEVEL_STRLEN 32
extern char LOG_LEVEL[MAX_LOGLEVEL_STRLEN];
#define LOG_LEVEL_DEFAULT "INFO"

#define MAX_HOST_STRLEN INET6_ADDRSTRLEN
extern char HOST[MAX_HOST_STRLEN];
#define HOST_DEFAULT "127.0.0.1"

extern int32_t PORT;
#define PORT_DEFAULT (int32_t)8000

int32_t load_configs(int argc, char* argv[]);

#endif /* CERIOUSDB_CONFIG_H */