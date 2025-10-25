#pragma once
#include <cstdio>

namespace pixl::core
{
    enum LogLevel
    {
        LOG_LEVEL_DEBUG = 0,
        LOG_LEVEL_INFO = 1,
        LOG_LEVEL_WARNING = 2,
        LOG_LEVEL_ERROR = 3,
        LOG_LEVEL_FATAL = 4
    };

    class Log
    {
    public:
        static void Init(LogLevel logLevel = LOG_LEVEL_ERROR, bool logToConsole = false, bool logToFile = false, const char *logFilePath = "pixl_engine.log");
        static int Quit();
        static void WriteLog(LogLevel level, const char *message);
        static void SetLogLevel(LogLevel level);

    private:
        static bool isInitialized_;
        static bool logToFile_;
        static bool logToConsole_;
        static FILE *logFile_;
        static LogLevel currentLogLevel_;
    };
}