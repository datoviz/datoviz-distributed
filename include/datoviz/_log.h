/**
 * Copyright (c) 2017 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>

#include "_macros.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

typedef void (*log_LockFn)(void* udata, int lock);

enum
{
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
};

#ifdef DEBUG
#define DVZ_DEFAULT_LOG_LEVEL LOG_TRACE
#else
#define DVZ_DEFAULT_LOG_LEVEL LOG_INFO
#endif

#define log_trace(...) log_log(LOG_TRACE, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILENAME__, __LINE__, __VA_ARGS__)

DVZ_EXPORT void log_set_udata(void* udata);
DVZ_EXPORT void log_set_lock(log_LockFn fn);
DVZ_EXPORT void log_set_fp(FILE* fp);
DVZ_EXPORT void log_set_level(int level);
DVZ_EXPORT void log_set_quiet(int enable);

DVZ_EXPORT void log_log(int level, const char* file, int line, const char* fmt, ...);

DVZ_EXPORT void log_set_level_env(void);

#ifdef __cplusplus
}
#endif

#endif
