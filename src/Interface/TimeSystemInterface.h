#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    class TimeSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TimeSystem" );

    public:
        virtual Timestamp getSystemTimestamp() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TIME_SYSTEM()\
    ((Mengine::TimeSystemInterface*)SERVICE_GET(Mengine::TimeSystemInterface))
//////////////////////////////////////////////////////////////////////////