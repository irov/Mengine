#include "POSIXDateTimeSystem.h"

#include <ctime>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( DateTimeSystem, Mengine::POSIXDateTimeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const uint64_t MILLISECONDS_SECOND64 = 1000ULL;
    static const uint64_t MILLISECONDS_MINUTE64 = MILLISECONDS_SECOND64 * 60ULL;
    static const uint64_t MILLISECONDS_HOUR64 = MILLISECONDS_MINUTE64 * 60ULL;
    static const uint64_t MILLISECONDS_DAY64 = MILLISECONDS_HOUR64 * 24ULL;
    static const uint64_t MILLISECONDS_MONTH64 = MILLISECONDS_DAY64 * 31ULL;
    static const uint64_t MILLISECONDS_YEAR64 = MILLISECONDS_MONTH64 * 12ULL;
    //////////////////////////////////////////////////////////////////////////
    POSIXDateTimeSystem::POSIXDateTimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXDateTimeSystem::~POSIXDateTimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXDateTimeSystem::getLocalDateTime( PlatformDateTime * const _dateTime ) const
    {
        std::time_t ctTime = std::time( nullptr );
        std::tm * sTime = std::localtime( &ctTime );

        _dateTime->year = 1900 + sTime->tm_year;
        _dateTime->month = sTime->tm_mon + 1;
        _dateTime->day = sTime->tm_mday;
        _dateTime->hour = sTime->tm_hour;
        _dateTime->minute = sTime->tm_min;
        _dateTime->second = sTime->tm_sec;
        _dateTime->milliseconds = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    TimeMilliseconds POSIXDateTimeSystem::getLocalDateTimeMilliseconds() const
    {
        PlatformDateTime date;
        this->getLocalDateTime( &date );

        uint64_t year64 = date.year;
        uint64_t month64 = date.month - 1;
        uint64_t day64 = date.day - 1;
        uint64_t hour64 = date.hour;
        uint64_t minute64 = date.minute;
        uint64_t second64 = date.second;
        uint64_t milliseconds = date.milliseconds;

        uint64_t milliseconds_year64 = year64 * MILLISECONDS_YEAR64;
        uint64_t milliseconds_month64 = month64 * MILLISECONDS_MONTH64;
        uint64_t milliseconds_day64 = day64 * MILLISECONDS_DAY64;
        uint64_t milliseconds_hour64 = hour64 * MILLISECONDS_HOUR64;
        uint64_t milliseconds_minute64 = minute64 * MILLISECONDS_MINUTE64;
        uint64_t milliseconds_second64 = second64 * MILLISECONDS_SECOND64;

        TimeMilliseconds time = milliseconds
            + milliseconds_second64
            + milliseconds_minute64
            + milliseconds_hour64
            + milliseconds_day64
            + milliseconds_month64
            + milliseconds_year64;

        return time;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXDateTimeSystem::getLocalDateTimeFromMilliseconds( TimeMilliseconds _time, PlatformDateTime * const _dateTime ) const
    {
        PlatformDateTime date;
        date.year = (uint32_t)(_time / MILLISECONDS_YEAR64);
        date.month = (uint32_t)((_time % MILLISECONDS_YEAR64) / MILLISECONDS_MONTH64) + 1;
        date.day = (uint32_t)((_time % MILLISECONDS_MONTH64) / MILLISECONDS_DAY64) + 1;
        date.hour = (uint32_t)((_time % MILLISECONDS_DAY64) / MILLISECONDS_HOUR64);
        date.minute = (uint32_t)((_time % MILLISECONDS_HOUR64) / MILLISECONDS_MINUTE64);
        date.second = (uint32_t)((_time % MILLISECONDS_MINUTE64) / MILLISECONDS_SECOND64);
        date.milliseconds = (uint32_t)(_time % MILLISECONDS_SECOND64);

        *_dateTime = date;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t POSIXDateTimeSystem::getTimeZoneOffset() const
    {
        const std::time_t epoch_plus_11h = 60 * 60 * 11;

        struct tm * lt = std::localtime( &epoch_plus_11h );
        struct tm * t = std::gmtime( &epoch_plus_11h );

        int32_t local_time = lt->tm_hour;
        int32_t gm_time = t->tm_hour;
        int32_t tz_diff = gm_time - local_time;
        int32_t tz_diff60 = tz_diff * 60;

        return tz_diff60;
    }
    //////////////////////////////////////////////////////////////////////////
}
