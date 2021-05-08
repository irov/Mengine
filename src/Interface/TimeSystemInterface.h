#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TimeSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TimeSystem" );

    public:
        virtual uint64_t getTimeMilliseconds() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TIME_SYSTEM()\
    ((Mengine::TimeSystemInterface*)SERVICE_GET(Mengine::TimeSystemInterface))
//////////////////////////////////////////////////////////////////////////