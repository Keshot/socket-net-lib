#include "error_handler.h"

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR_BUFFER_LN 1024

void err_crit(const char *errmsg)
{
    const char *ret_buff = nullptr;
    char buff[ERROR_BUFFER_LN];
    ret_buff = strerror_r(errno, buff, ERROR_BUFFER_LN);

    fprintf(stderr, "CRITICAL ERROR: %s\terror additional info: %s.\n", ret_buff, errmsg);

    exit(EXIT_FAILURE);
}

void err_log(const char *errmsg)
{
    const char *ret_buff = nullptr;
    char buff[ERROR_BUFFER_LN];
    ret_buff = strerror_r(errno, buff, ERROR_BUFFER_LN);
    fprintf(stderr, "ERROR: %s\terror additional info: %s.\n", ret_buff, errmsg);
}