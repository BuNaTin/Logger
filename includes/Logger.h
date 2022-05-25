#pragma once

#include "../src/LoggerImpl.h"

/*
 *   if defined LOG_WITH_TIME, returns char[] time,
 *   or returns '\0' in other way;
 */
char *get_time();

#define LOGI()                                                         \
    if (Logger::lvl::debug == Logger::log_lvl)                         \
    Logger() << get_time() << "[INFO] "

#define LOGW()                                                         \
    if (Logger::lvl::debug == Logger::log_lvl)                         \
    Logger() << get_time() << "[WARN] "

#define LOGE() Logger() << get_time() << "[ERR] "

/*
 *   RAII class to ensure Logger::lvl::debug
 *   in concrete scope set
 */
class LoggerDebug {
public:
    LoggerDebug();
    ~LoggerDebug();
};
