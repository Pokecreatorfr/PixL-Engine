#include <core/Time.hpp>

using namespace pixl::core;

bool Time::isInitialized_ = false;
std::chrono::steady_clock::time_point Time::startTime_;

int Time::Init()
{
    if (isInitialized_)
    {
        return 0;
    }

    startTime_ = std::chrono::steady_clock::now();
    isInitialized_ = true;
    return 0;
}

int Time::Quit()
{
    if (!isInitialized_)
    {
        return 0;
    }
    return 0;
}

double Time::GetTime()
{
    if (!isInitialized_)
    {
        return 0.0;
    }

    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - startTime_;
    return elapsed.count();
}

DateTime Time::GetDateTime()
{
    DateTime dateTime{};
    std::time_t t = std::time(nullptr);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &t);
#else
    localtime_r(&t, &localTime);
#endif

    dateTime.year = static_cast<uint16_t>(localTime.tm_year + 1900);
    dateTime.month = static_cast<Month>(localTime.tm_mon + 1);
    dateTime.day = static_cast<uint8_t>(localTime.tm_mday);
    dateTime.weekDay = static_cast<WeekDay>(localTime.tm_wday);
    dateTime.hours = static_cast<uint8_t>(localTime.tm_hour);
    dateTime.minutes = static_cast<uint8_t>(localTime.tm_min);
    dateTime.seconds = static_cast<uint8_t>(localTime.tm_sec);

#ifdef _WIN32
    std::tm gm{};
    gmtime_s(&gm, &t);
    long diff = static_cast<long>(std::difftime(std::mktime(&localTime), _mkgmtime(&gm)));
    dateTime.utcOffset = static_cast<int16_t>(diff / 60);
#else
    dateTime.utcOffset = static_cast<int16_t>(localTime.tm_gmtoff / 60);
#endif

    return dateTime;
}
