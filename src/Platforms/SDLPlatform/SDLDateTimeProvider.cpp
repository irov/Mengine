#include "SDLDateTimeProvider.h"

#include "SDL_timer.h"

#include "Kernel/AssertionNotImplemented.h"

#include <ctime>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLDateTimeProvider::SDLDateTimeProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLDateTimeProvider::~SDLDateTimeProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLDateTimeProvider::getLocalDateTime( PlatformDateTime * _dateTime ) const
    {
        std::time_t ctTime = std::time( nullptr );
        std::tm * sTime = std::localtime( &ctTime );

        _dateTime->year = 1900 + sTime->tm_year;
        _dateTime->month = sTime->tm_mon + 1;
        _dateTime->day = sTime->tm_mday;
        _dateTime->hour = sTime->tm_hour;
        _dateTime->minute = sTime->tm_min;
        _dateTime->second = sTime->tm_sec;
        _dateTime->milliseconds = SDL_GetTicks();
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t SDLDateTimeProvider::getLocalDateMilliseconds() const
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
    //////////////////////////////////////////////////////////////////////////
    int32_t SDLDateTimeProvider::getTimeZoneOffset() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
}