#include "Win32DateTimeSystem.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Helper.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( DateTimeSystem, Mengine::Win32DateTimeSystem );
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
    Win32DateTimeSystem::Win32DateTimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32DateTimeSystem::~Win32DateTimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32DateTimeSystem::getLocalDateTime( PlatformDateTime * const _dateTime ) const
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
    Timestamp Win32DateTimeSystem::getLocalDateTimeMilliseconds() const
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

        Timestamp time = milliseconds
            + milliseconds_second64
            + milliseconds_minute64
            + milliseconds_hour64
            + milliseconds_day64
            + milliseconds_month64
            + milliseconds_year64;

        return time;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32DateTimeSystem::getLocalDateTimeFromMilliseconds( Timestamp _time, PlatformDateTime * const _dateTime ) const
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
    int32_t Win32DateTimeSystem::getTimeZoneOffset() const
    {
        TIME_ZONE_INFORMATION TimeZoneInformation;

        DWORD zoneId = ::GetTimeZoneInformation( &TimeZoneInformation );

        switch( zoneId )
        {
        case TIME_ZONE_ID_UNKNOWN:
        case TIME_ZONE_ID_STANDARD:
            {
                LONG zoneBias = TimeZoneInformation.Bias + TimeZoneInformation.StandardBias;

                return zoneBias;
            }break;
        case TIME_ZONE_ID_DAYLIGHT:
            {
                LONG zoneBias = TimeZoneInformation.Bias + TimeZoneInformation.DaylightBias;

                return zoneBias;
            }break;
        case TIME_ZONE_ID_INVALID:
            {
                LOGGER_ASSERTION( "GetTimeZoneInformation get error %ls"
                    , Helper::Win32GetLastErrorMessage()
                );

                return 0;
            }break;
        }

        LOGGER_ASSERTION( "GetTimeZoneInformation unknown zone ID [%lu]"
            , zoneId
        );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
