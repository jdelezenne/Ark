#include "Ark/Core/DateTime.hpp"

#include <ctime>

namespace Ark::DateTimeConvert
{
    namespace
    {
        bool isLeap(int year)
        {
            if ((year % 4) != 0)
            {
                return false;
            }

            if ((year % 100) != 0)
            {
                return true;
            }

            return (year % 400) == 0;
        }

        long long daysFromCivil(int y, unsigned m, unsigned d)
        {
            y -= (m <= 2 ? 1 : 0);
            long long era = (y >= 0 ? y : y - 399) / 400;
            unsigned yoe = static_cast<unsigned>(y - era * 400);
            unsigned doy = (153 * (m + (m > 2 ? -3u : 9u)) + 2) / 5 + d - 1;
            unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + yoe / 400 + doy;
            return era * 146097 + static_cast<long long>(doe) - 719468;
        }
    }

    int getDaysInMonth(int year, int month)
    {
        if (month < 1 || month > 12)
        {
            return 0;
        }

        static int const days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        if (month == 2 && isLeap(year))
        {
            return 29;
        }

        return days[month - 1];
    }

    int getDayOfYear(int year, int month, int day)
    {
        if (month < 1 || month > 12)
        {
            return -1;
        }

        int const dim = getDaysInMonth(year, month);
        if (day < 1 || day > dim)
        {
            return -1;
        }

        int doy = 0;
        for (int m = 1; m < month; ++m)
        {
            doy += getDaysInMonth(year, m);
        }

        doy += (day - 1);
        return doy;
    }

    int getDayOfWeek(int year, int month, int day)
    {
        if (month < 3)
        {
            month += 12;
            --year;
        }

        int yearOfCentury = year % 100;
        int century = year / 100;
        int h = (day + (13 * (month + 1)) / 5 + yearOfCentury + (yearOfCentury / 4) + (century / 4) + 5 * century) % 7;
        int weekday = (h + 6) % 7;
        return weekday;
    }

    Result<uint64> toUnixMilliseconds(DateTime const& dateTimeUtc)
    {
        int year = static_cast<int>(dateTimeUtc.Date.Year);
        int month = static_cast<int>(dateTimeUtc.Date.Month);
        int day = static_cast<int>(dateTimeUtc.Date.Day);

        long long const days = daysFromCivil(year, static_cast<unsigned>(month), static_cast<unsigned>(day));
        long long seconds = days * 86400LL;
        seconds += static_cast<long long>(dateTimeUtc.Time.Hour) * 3600LL;
        seconds += static_cast<long long>(dateTimeUtc.Time.Minute) * 60LL;
        seconds += static_cast<long long>(dateTimeUtc.Time.Second);
        if (seconds < 0)
        {
            return Result<uint64>(0ULL);
        }

        uint64 const ms = static_cast<uint64>(seconds) * 1000ULL + static_cast<uint64>(dateTimeUtc.Time.Millisecond);
        return Result<uint64>(ms);
    }

    Result<DateTime> fromUnixMilliseconds(uint64 unixMs, bool localTime)
    {
        time_t const seconds = static_cast<time_t>(unixMs / 1000ULL);
        int const millis = static_cast<int>(unixMs % 1000ULL);

        std::tm tm{};
        if (localTime)
        {
#if defined(_WIN32)
            localtime_s(&tm, &seconds);
#else
            localtime_r(&seconds, &tm);
#endif
        }
        else
        {
#if defined(_WIN32)
            gmtime_s(&tm, &seconds);
#else
            gmtime_r(&seconds, &tm);
#endif
        }

        DateTime dt{};
        dt.Date.Year = static_cast<uint32>(tm.tm_year + 1900);
        dt.Date.Month = static_cast<Month>(tm.tm_mon + 1);
        dt.Date.Day = static_cast<uint32>(tm.tm_mday);
        dt.Date.Weekday = static_cast<Weekday>(tm.tm_wday);
        dt.Time.Hour = static_cast<uint32>(tm.tm_hour);
        dt.Time.Minute = static_cast<uint32>(tm.tm_min);
        dt.Time.Second = static_cast<uint32>(tm.tm_sec);
        dt.Time.Millisecond = static_cast<uint32>(millis);
        return Result<DateTime>(dt);
    }
}
