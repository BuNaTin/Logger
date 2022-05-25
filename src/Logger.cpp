#include "Logger.h"

#include <ctime>
#include <ostream>

char *get_time() {
    static char nullsymb = '\0';
#if defined(LOG_WITH_TIME)
    time_t now = time(nullptr);
    char *curTimeAsStr = ctime(&now);
    curTimeAsStr[24] = ' ';
    return curTimeAsStr;
#endif

    return &nullsymb;
}

Logger::lvl Logger::log_lvl = lvl::release;

struct NullOutput : public std::ostream {};

NullOutput nullOutput;

std::ostream *Logger::output = &nullOutput;

/*
 *   RAII class to ensure Logger::lvl::debug
 *   in concrete scope set
 */
LoggerDebug::LoggerDebug() { Logger::setLogLvl(Logger::lvl::debug); }
LoggerDebug::~LoggerDebug() { Logger::setLogLvl(Logger::lvl::release); }