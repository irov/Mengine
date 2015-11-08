#   include "MarmaladeTimer.h"

#	include <ctime>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MarmaladeTimer::MarmaladeTimer() 
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MarmaladeTimer::~MarmaladeTimer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t MarmaladeTimer::getMilliseconds()
    {
        uint64 ms = s3eTimerGetMs();
        uint64_t ul_ms = static_cast<uint64_t>(ms);

        return ul_ms;
    }
}