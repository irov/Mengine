#include "Win32DateTimeProvider.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    void Win32DateTimeProvider::getDateTime( PlatformDateTime * _dateTime ) const
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
}