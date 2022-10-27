#include<stdarg.h>
#include<time.h>
#include<stdio.h>

#define LOG_LEVEL 1

#define ERROR 1
#define WARN 2
#define INFO 3
#define DEBUG 4


// usage: LogRecord(ERROR, "%s is error", "main");
void LogRecord(int level, const char *format,...) {
    if (level < LOG_LEVEL) {
        return ;
    }

    va_list args;
    int done;
    va_start(args, format);
    time_t log_time = time(NULL);
    struct tm* tm_log = localtime(&log_time);
    switch (level) {
    case ERROR:
        printf("\033[0m\033[1;31m[ERROR]: \033[0m");
        break;
    case WARN:
        printf("\033[0m\033[1;33m[WARN]: \033[0m");
        break;
    case INFO:
        printf("\033[0m\033[1;34m[INFO]: \033[0m");
        break;
    case DEBUG:
        printf("[DEBUG]: ");
        break;
    default:
        break;
    }

    printf("%04d-%02d-%02d %02d:%02d:%02d ", tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);
    vfprintf(stdout, format, args);
    printf("\n");
    va_end(args);

    return ;
}