#ifndef CERIOUSDB_INTERFACE_H
#define CERIOUSDB_INTERFACE_H

/*
* options / commands:
* - q / stop -> stop server (terminate process with all threads)
*/

#include <stdint.h>

void interface_printline(void);

/* starts the server interface, returns 0 on success, -1 on error */
int32_t server_interface(void);

#endif /* CERIOUSDB_INTERFACE_H */