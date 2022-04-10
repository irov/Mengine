#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Time.h"

namespace Mengine
{
    class TimeSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TimeSystem" );

    public:
        virtual TimeMilliseconds getTimeMilliseconds() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TIME_SYSTEM()\
    ((Mengine::TimeSystemInterface*)SERVICE_GET(Mengine::TimeSystemInterface))
//////////////////////////////////////////////////////////////////////////