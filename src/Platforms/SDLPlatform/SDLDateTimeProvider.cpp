#include "SDLDateTimeProvider.h"

#include "SDL_timer.h"

#include <ctime>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    void SDLDateTimeProvider::getDateTime( PlatformDateTime * _dateTime ) const
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
}