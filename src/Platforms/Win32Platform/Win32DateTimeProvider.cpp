#include "Win32DateTimeProvider.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    void Win32DateTimeProvider::getLocalDateTime( PlatformDateTime * _dateTime ) const
    {
        SYSTEMTIME time;
        ::GetLocalTime( &time );

        _dateTime->year = time.wYear;
        _dateTime->month = time.wMonth;
        _dateTime->day = time.wDay;
        _dateTime->hour = time.wHour;
        _dateTime->minute = time.wMinute;
        _dateTime->second = time.wSecond;
        _dateTime->milliseconds = time.wMilliseconds;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t Win32DateTimeProvider::getLocalDateMilliseconds() const
    {
        PlatformDateTime date;
        this->getLocalDateTime( &date );

        uint64_t year64 = date.year;
        uint64_t month64 = date.month;
        uint64_t day64 = date.day;
        uint64_t hour64 = date.hour;
        uint64_t minute64 = date.minute;
        uint64_t second64 = date.second;
        uint64_t milliseconds = date.milliseconds;

        uint64_t milliseconds_year64 = year64 * 1000ULL * 60ULL * 60ULL * 30ULL * 365ULL;
        uint64_t milliseconds_month64 = month64 * 1000ULL * 60ULL * 60ULL * 30ULL;
        uint64_t milliseconds_day64 = day64 * 1000ULL * 60ULL * 60ULL * 24ULL;
        uint64_t milliseconds_hour64 = hour64 * 1000ULL * 60ULL * 60ULL;
        uint64_t milliseconds_minute64 = minute64 * 1000ULL * 60ULL;
        uint64_t milliseconds_second64 = second64 * 1000ULL;

        uint64_t total = milliseconds 
            + milliseconds_second64
            + milliseconds_minute64
            + milliseconds_hour64
            + milliseconds_day64
            + milliseconds_month64
            + milliseconds_year64;

        return total;
    }
}