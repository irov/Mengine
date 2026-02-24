#include "POSIXDateTimeSystem.h"

#include <time.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( DateTimeSystem, Mengine::POSIXDateTimeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void convertTMToDateTime( const tm & _tm, uint32_t _milliseconds, PlatformDateTime * const _dateTime )
        {
            _dateTime->year = 1900 + _tm.tm_year;
            _dateTime->month = _tm.tm_mon + 1;
            _dateTime->day = _tm.tm_mday;
            _dateTime->hour = _tm.tm_hour;
            _dateTime->minute = _tm.tm_min;
            _dateTime->second = _tm.tm_sec;
            _dateTime->milliseconds = _milliseconds;
        }
        //////////////////////////////////////////////////////////////////////////
        static void localTime( time_t _time, tm * const _tm )
        {
#if defined(MENGINE_PLATFORM_ANDROID) || defined(MENGINE_VENDOR_APPLE) || defined(MENGINE_PLATFORM_LINUX)
            ::localtime_r( &_time, _tm );
#else
            ::localtime_s( _tm, &_time );
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        static void gmTime( time_t _time, tm * const _tm )
        {
#if defined(MENGINE_PLATFORM_ANDROID) || defined(MENGINE_VENDOR_APPLE) || defined(MENGINE_PLATFORM_LINUX)
            ::gmtime_r( &_time, _tm );
#else
            ::gmtime_s( _tm, &_time );
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        static void getTime( timespec * const _ts )
        {
#if defined(MENGINE_PLATFORM_ANDROID) || defined(MENGINE_VENDOR_APPLE) || defined(MENGINE_PLATFORM_LINUX)
            ::clock_gettime( CLOCK_REALTIME, _ts );
#else
            ::timespec_get( _ts, TIME_UTC );
#endif
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXDateTimeSystem::POSIXDateTimeSystem()
        : m_timeZoneOffset( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXDateTimeSystem::~POSIXDateTimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXDateTimeSystem::_initializeService()
    {
        time_t t = ::time( nullptr );

        tm sLocalTime = {0};
        Detail::localTime( t, &sLocalTime );

        tm sGMTime = {0};
        Detail::gmTime( t, &sGMTime );

        time_t ctLocalTime = ::mktime( &sLocalTime );
        time_t ctGMTime = ::mktime( &sGMTime );

        int64_t timeZoneOffset = (int64_t)(ctGMTime - ctLocalTime) * 1000;

        m_timeZoneOffset = timeZoneOffset;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXDateTimeSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXDateTimeSystem::getSystemDateTime( PlatformDateTime * const _dateTime ) const
    {
        struct timespec tv = {0};
        Detail::getTime( &tv );

        time_t t = (time_t)tv.tv_sec;

        tm sTime = {0};
        Detail::gmTime( t, &sTime );
        
        uint32_t milliseconds = (uint32_t)(tv.tv_nsec / 1000000);

        Detail::convertTMToDateTime( sTime, milliseconds, _dateTime );
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXDateTimeSystem::getLocalDateTime( PlatformDateTime * const _dateTime ) const
    {
        struct timespec tv = {0};
        Detail::getTime( &tv );

        time_t t = (time_t)tv.tv_sec;

        tm sTime = {0};
        Detail::localTime( t, &sTime );
        
        uint32_t milliseconds = (uint32_t)(tv.tv_nsec / 1000000);

        Detail::convertTMToDateTime( sTime, milliseconds, _dateTime );
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXDateTimeSystem::getDateTimeFromMilliseconds( Timestamp _time, PlatformDateTime * const _dateTime ) const
    {
        time_t ctTime = _time / 1000;

        tm sTime = {0};
        Detail::gmTime( ctTime, &sTime );

        uint32_t milliseconds = (uint32_t)(_time % 1000);

        Detail::convertTMToDateTime( sTime, milliseconds, _dateTime );
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t POSIXDateTimeSystem::getTimeZoneOffsetMilliseconds() const
    {
        return m_timeZoneOffset;
    }
    //////////////////////////////////////////////////////////////////////////
}
