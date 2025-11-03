#pragma once
#include <chrono>
#include <cstdint>
#include <string>

namespace pixl::core
{
    enum class WeekDay : uint8_t
    {
        Sunday = 0,
        Monday = 1,
        Tuesday = 2,
        Wednesday = 3,
        Thursday = 4,
        Friday = 5,
        Saturday = 6
    };

    enum class Month : uint8_t
    {
        January = 1,
        February = 2,
        March = 3,
        April = 4,
        May = 5,
        June = 6,
        July = 7,
        August = 8,
        September = 9,
        October = 10,
        November = 11,
        December = 12
    };

    struct DateTime
    {
        uint16_t year;
        Month month;
        uint8_t day;
        WeekDay weekDay;
        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;
        int16_t utcOffset;
    };

    class Time
    {
    public:
        static int Init();
        static int Quit();

        static double GetTime();
        static DateTime GetDateTime();

    private:
        static bool isInitialized_;
        static std::chrono::steady_clock::time_point startTime_;
    };
}