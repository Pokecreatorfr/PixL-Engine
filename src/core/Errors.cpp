#include <core/Errors.hpp>

using namespace pixl::core;

bool Errors::IsInitialized_ = false;

int pixl::core::Errors::Init()
{
    if (IsInitialized_)
    {
        return 0;
    }

    Log::Init();

    IsInitialized_ = true;

    return 0;
}

int pixl::core::Errors::Quit()
{
    if (!IsInitialized_)
    {
        return 0;
    }
    IsInitialized_ = false;

    return 0;
}

void pixl::core::Errors::LogError(const ErrorLog &error)
{
    if (!IsInitialized_)
    {
        return;
    }

    std::string logMessage = "Error Code: " + std::to_string(static_cast<uint32_t>(error.code)) +
                             ", Message: " + error.message +
                             ", Module: " + std::string(error.module) +
                             ", Location: " + std::string(error.where.file_name()) + ":" +
                             std::to_string(error.where.line());

    if (error.cause.has_value())
    {
        logMessage += ", Cause: " + error.cause.value();
    }

    Log::WriteLog(LOG_LEVEL_ERROR, logMessage.c_str());
}