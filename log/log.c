#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
void log_init()
{
    FILE *f = fopen("log.txt", "w");
    fclose(f);
}
void Log(char *TAG, char *type, char *msg)
{
    FILE *f = fopen("log.txt", "a");
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(f, "[%d-%d-%d %d:%d:%d] [%s] [%s] %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, TAG, type, msg);
    fclose(f);
}
