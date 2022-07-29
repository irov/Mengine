#include "Win32DateTimeProvider.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Logger.h"
#include "Kernel/Win32Helper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32DateTimeProvider::Win32DateTimeProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32DateTimeProvider::~Win32DateTimeProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32DateTimeProvider::getLocalDateTime( PlatformDateTime * const _dateTime ) const
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
    //////////////////////////////////////////////////////////////////////////
    int32_t Win32DateTimeProvider::getTimeZoneOffset() const
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
                LOGGER_ERROR( "GetTimeZoneInformation get error %ls"
                    , Helper::Win32GetLastErrorMessage()
                );

                return 0;
            }break;
        }

        LOGGER_ERROR( "GetTimeZoneInformation unknown zone ID [%lu]"
            , zoneId
        );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
}