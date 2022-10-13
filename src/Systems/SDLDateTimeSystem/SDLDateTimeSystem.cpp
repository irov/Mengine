#include "SDLDateTimeSystem.h"

#include <ctime>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( DateTimeSystem, Mengine::SDLDateTimeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLDateTimeSystem::SDLDateTimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLDateTimeSystem::~SDLDateTimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLDateTimeSystem::getLocalDateTime( PlatformDateTime * const _dateTime ) const
    {
        std::time_t ctTime = std::time( nullptr );
        std::tm * sTime = std::localtime( &ctTime );

        _dateTime->year = 1900 + sTime->tm_year;
        _dateTime->month = sTime->tm_mon;
        _dateTime->day = sTime->tm_mday;
        _dateTime->hour = sTime->tm_hour;
        _dateTime->minute = sTime->tm_min;
        _dateTime->second = sTime->tm_sec;
        _dateTime->milliseconds = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t SDLDateTimeSystem::getLocalDateMilliseconds() const
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

        uint64_t milliseconds_year64 = year64 * 1000ULL * 60ULL * 60ULL * 24ULL * 31ULL * 12ULL;
        uint64_t milliseconds_month64 = month64 * 1000ULL * 60ULL * 60ULL * 24ULL * 31ULL;
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
    void SDLDateTimeSystem::getLocalDateTimeFromMilliseconds( uint64_t _timestamp, PlatformDateTime * const _dateTime ) const
    {
        uint64_t milliseconds_year64 = 1000ULL * 60ULL * 60ULL * 24ULL * 31ULL * 12ULL;
        uint64_t milliseconds_month64 = 1000ULL * 60ULL * 60ULL * 24ULL * 31ULL;
        uint64_t milliseconds_day64 = 1000ULL * 60ULL * 60ULL * 24ULL;
        uint64_t milliseconds_hour64 = 1000ULL * 60ULL * 60ULL;
        uint64_t milliseconds_minute64 = 1000ULL * 60ULL;
        uint64_t milliseconds_second64 = 1000ULL;

        PlatformDateTime date;
        date.year = (uint32_t)(_timestamp / milliseconds_year64);
        date.month = (uint32_t)((_timestamp % milliseconds_year64) / milliseconds_month64);
        date.day = (uint32_t)((_timestamp % milliseconds_month64) / milliseconds_day64);
        date.hour = (uint32_t)((_timestamp % milliseconds_day64) / milliseconds_hour64);
        date.minute = (uint32_t)((_timestamp % milliseconds_hour64) / milliseconds_minute64);
        date.second = (uint32_t)((_timestamp % milliseconds_minute64) / milliseconds_second64);
        date.milliseconds = (uint32_t)(_timestamp % milliseconds_second64);

        *_dateTime = date;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDLDateTimeSystem::getTimeZoneOffset() const
    {
        const std::time_t epoch_plus_11h = 60 * 60 * 11;

        struct tm * lt = ::localtime( &epoch_plus_11h );
        struct tm * t = ::gmtime( &epoch_plus_11h );

        int32_t local_time = lt->tm_hour;
        int32_t gm_time = t->tm_hour;
        int32_t tz_diff = gm_time - local_time;
        int32_t tz_diff60 = tz_diff * 60;

        return tz_diff60;
    }
    //////////////////////////////////////////////////////////////////////////
}
