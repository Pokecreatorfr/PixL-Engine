#include <core/Log.hpp>

using namespace pixl::core;

bool Log::isInitialized_ = false;
bool Log::logToFile_ = false;
bool Log::logToConsole_ = false;
FILE *Log::logFile_ = nullptr;
LogLevel Log::currentLogLevel_ = LOG_LEVEL_ERROR;

void pixl::core::Log::Init(LogLevel logLevel, bool logToConsole, bool logToFile, const char *logFilePath)
{
    if (isInitialized_)
    {
        return;
    }

    currentLogLevel_ = logLevel;
    logToConsole_ = logToConsole;
    logToFile_ = logToFile;

    if (logToFile_)
    {
        logFile_ = fopen(logFilePath, "a");
        if (logFile_ == nullptr)
        {
            logToFile_ = false;
            if (logToConsole_)
            {
                isInitialized_ = true;
                WriteLog(LOG_LEVEL_ERROR, "Failed to open log file for writing.");
            }
        }
    }

    isInitialized_ = true;
}

int pixl::core::Log::Quit()
{
    if (!isInitialized_)
    {
        return 0;
    }

    if (logFile_ != nullptr)
    {
        fclose(logFile_);
        logFile_ = nullptr;
    }

    isInitialized_ = false;
    return 0;
}

void pixl::core::Log::WriteLog(LogLevel level, const char *message)
{
    if (level < currentLogLevel_ || !isInitialized_)
    {
        return;
    }

    const char *levelStr = "";
    switch (level)
    {
    case LOG_LEVEL_DEBUG:
        levelStr = "DEBUG";
        break;
    case LOG_LEVEL_INFO:
        levelStr = "INFO";
        break;
    case LOG_LEVEL_WARNING:
        levelStr = "WARNING";
        break;
    case LOG_LEVEL_ERROR:
        levelStr = "ERROR";
        break;
    case LOG_LEVEL_FATAL:
        levelStr = "FATAL";
        break;
    }

    if (logToConsole_)
    {
        printf("[%s] %s\n", levelStr, message);
    }

    if (logFile_ != nullptr && logToFile_)
    {
        fprintf(logFile_, "[%s] %s\n", levelStr, message);
        fflush(logFile_);
    }
}

void pixl::core::Log::SetLogLevel(LogLevel level)
{
    currentLogLevel_ = level;
}