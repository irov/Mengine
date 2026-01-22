#include "POSIXTimeSystem.h"

#include <time.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimeSystem, Mengine::POSIXTimeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    POSIXTimeSystem::POSIXTimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXTimeSystem::~POSIXTimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXTimeSystem::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXTimeSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp POSIXTimeSystem::getSystemTimestamp() const
    {
        struct timespec tv;

#if defined(MENGINE_PLATFORM_ANDROID) || defined(MENGINE_PLATFORM_APPLE)
        ::clock_gettime( CLOCK_REALTIME, &tv );
#else
        ::timespec_get( &tv, TIME_UTC );
#endif

        Timestamp milliseconds = tv.tv_sec * MENGINE_INT64_C( 1000 ) + tv.tv_nsec / MENGINE_INT64_C( 1000000 );

        return milliseconds;
    }
    //////////////////////////////////////////////////////////////////////////
    double POSIXTimeSystem::getElapsedTime() const
    {
        struct timespec tv;

#if defined(MENGINE_PLATFORM_ANDROID) || defined(MENGINE_PLATFORM_APPLE)
        ::clock_gettime( CLOCK_MONOTONIC, &tv );
#else
        ::timespec_get( &tv, TIME_UTC );
#endif

        double milliseconds = (double)tv.tv_sec * 1000.0 + (double)tv.tv_nsec / 1000000.0;

        return milliseconds;
    }
    //////////////////////////////////////////////////////////////////////////
}
