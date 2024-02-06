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
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void convertSystemTimeToDateTime( const SYSTEMTIME & _systemTime, PlatformDateTime * const _dateTime )
        {
            _dateTime->year = _systemTime.wYear;
            _dateTime->month = _systemTime.wMonth;
            _dateTime->day = _systemTime.wDay;
            _dateTime->hour = _systemTime.wHour;
            _dateTime->minute = _systemTime.wMinute;
            _dateTime->second = _systemTime.wSecond;
            _dateTime->milliseconds = _systemTime.wMilliseconds;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Win32DateTimeSystem::Win32DateTimeSystem()
        : m_timeZoneOffset( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32DateTimeSystem::~Win32DateTimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32DateTimeSystem::_initializeService()
    {
        SYSTEMTIME systemTime = {0};
        ::GetSystemTime( &systemTime );

        SYSTEMTIME localTime = {0};
        ::GetLocalTime( &localTime );

        FILETIME systemFileTime = {0};
        ::SystemTimeToFileTime( &systemTime, &systemFileTime );

        FILETIME localFileTime = {0};
        ::SystemTimeToFileTime( &localTime, &localFileTime );

        ULARGE_INTEGER uiSystemFileTime;
        uiSystemFileTime.LowPart = systemFileTime.dwLowDateTime;
        uiSystemFileTime.HighPart = systemFileTime.dwHighDateTime;

        ULARGE_INTEGER uiLocalFileTime;
        uiLocalFileTime.LowPart = localFileTime.dwLowDateTime;
        uiLocalFileTime.HighPart = localFileTime.dwHighDateTime;

        LONGLONG diffIn100Nanoseconds = uiSystemFileTime.QuadPart - uiLocalFileTime.QuadPart;
        LONGLONG diffInMilliseconds = diffIn100Nanoseconds / 10000;

        m_timeZoneOffset = diffInMilliseconds;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32DateTimeSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32DateTimeSystem::getSystemDateTime( PlatformDateTime * const _dateTime ) const
    {
        SYSTEMTIME systemTime = {0};
        ::GetSystemTime( &systemTime );

        Detail::convertSystemTimeToDateTime( systemTime, _dateTime );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32DateTimeSystem::getLocalDateTime( PlatformDateTime * const _dateTime ) const
    {
        SYSTEMTIME localTime = {0};
        ::GetLocalTime( &localTime );

        Detail::convertSystemTimeToDateTime( localTime, _dateTime );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32DateTimeSystem::getDateTimeFromMilliseconds( Timestamp _time, PlatformDateTime * const _dateTime ) const
    {
        ULARGE_INTEGER ull;
        ull.QuadPart = (static_cast<ULONGLONG>(_time) + 11644473600000ULL) * 10000ULL;

        FILETIME fileTime;
        fileTime.dwLowDateTime = ull.LowPart;
        fileTime.dwHighDateTime = ull.HighPart;

        SYSTEMTIME systemTime = {0};
        ::FileTimeToSystemTime( &fileTime, &systemTime );

        Detail::convertSystemTimeToDateTime( systemTime, _dateTime );
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t Win32DateTimeSystem::getTimeZoneOffsetMilliseconds() const
    {
        return m_timeZoneOffset;
    }
    //////////////////////////////////////////////////////////////////////////
}
