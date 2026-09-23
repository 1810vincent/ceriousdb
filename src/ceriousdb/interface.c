#define _POSIX_C_SOURCE 200809L

#include "interface.h"

#include <ceriousapi/logging.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

static time_t start_time = 0L;

static void init_uptime() {
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    start_time = current_time.tv_sec;
}

static time_t get_uptime() {
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    return (current_time.tv_sec - start_time);
}

#define MAX_UPTIME_STRLEN 64

static char* get_uptime_fstring() {
    char* uptime_fstring = (char*)malloc(MAX_UPTIME_STRLEN * sizeof(*uptime_fstring));
    if (!uptime_fstring)
        return NULL;

    time_t uptime = get_uptime();

    const time_t sec_per_year  = 365L * 24L * 3600L;
    const time_t sec_per_month = 30L * 24L * 3600L;
    const time_t sec_per_week  = 7L * 24L * 3600L;
    const time_t sec_per_day   = 24L * 3600L;
    const time_t sec_per_hour  = 3600L;
    const time_t sec_per_min   = 60L;

    /* filter */
    time_t years   = uptime / sec_per_year;
    time_t rem     = uptime % sec_per_year;
    
    time_t months  = rem / sec_per_month;
    rem           %= sec_per_month;

    time_t weeks   = rem / sec_per_week;
    rem           %= sec_per_week;
    
    time_t days    = rem / sec_per_day;
    rem           %= sec_per_day;
    
    time_t hours   = rem / sec_per_hour;
    rem           %= sec_per_hour;
    
    time_t minutes = rem / sec_per_min;
    time_t seconds = rem % sec_per_min;

    int32_t offset = 0;

    if (years > 0) {
        offset += snprintf(uptime_fstring + offset, MAX_UPTIME_STRLEN - offset, "%ldy ", years);
    }

    if (months > 0 || years > 0) {
        offset += snprintf(uptime_fstring + offset, MAX_UPTIME_STRLEN - offset, "%ldmo ", months);
    }

    if (weeks > 0 || months > 0 || years > 0) {
        offset += snprintf(uptime_fstring + offset, MAX_UPTIME_STRLEN - offset, "%ldw ", weeks);
    }

    if (days > 0 || weeks > 0 || months > 0 || years > 0) {
        offset += snprintf(uptime_fstring + offset, MAX_UPTIME_STRLEN - offset, "%ldd ", days);
    }

    if (hours > 0 || days > 0 || weeks > 0 || months > 0 || years > 0) {
        offset += snprintf(uptime_fstring + offset, MAX_UPTIME_STRLEN - offset, "%ldh ", hours);
    }

    if (minutes > 0 || hours > 0 || days > 0 || weeks > 0 || months > 0 || years > 0) {
        offset += snprintf(uptime_fstring + offset, MAX_UPTIME_STRLEN - offset, "%ldm ", minutes);
    }

    snprintf(uptime_fstring + offset, MAX_UPTIME_STRLEN - offset, "%lds", seconds);

    return uptime_fstring;
}

void interface_printline(void) {
    char* uptime_fstring = get_uptime_fstring();
    if (!uptime_fstring)
        return;
    printf("uptime: %s > ", uptime_fstring);
    fflush(stdout);
    free(uptime_fstring);
    return;
}

static bool interpret_command(char* input_buffer) {
    if ((strcmp(input_buffer, "q") == 0) || (strcmp(input_buffer, "stop") == 0))
        return ceriousapi_rtlog_server_info("stopping server...", "CeriousDB/Interface");
    else
        printf("\33[31munknown command: %s\33[0m\n", input_buffer);
    return false;
}

#define INTERFACE_STARTUP_DELAY_NSEC 10000000L

#define USERINPUT_USEC 500000L

#define MAXINPUT_LEN 1024

int32_t server_interface(void) {

    init_uptime();

    struct timespec sleeptime = {
        .tv_sec = 0,
        .tv_nsec = INTERFACE_STARTUP_DELAY_NSEC
    };
    nanosleep(&sleeptime, NULL);

    char input_buffer[MAXINPUT_LEN];
    while (true) {
        interface_printline();

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = USERINPUT_USEC;

        int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

        if (ret < 0)
            break;
        else if (ret == 0) {
            printf("\r\33[2K");
            continue;
        }
        else {
            if (!fgets(input_buffer, MAXINPUT_LEN, stdin)) {
                continue;
            }
            input_buffer[strcspn(input_buffer, "\n")] = '\0';

            bool stop_server = interpret_command(input_buffer);
            if (stop_server) {
                break;
            }
        }
    }
    return (int32_t)0;
}