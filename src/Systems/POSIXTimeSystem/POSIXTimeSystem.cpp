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

        Timestamp milliseconds = tv.tv_sec * 1000LL + tv.tv_nsec / 1000000;

        return milliseconds;
    }
    //////////////////////////////////////////////////////////////////////////
}
