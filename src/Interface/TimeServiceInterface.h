#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class TimeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TimeService" );

    public:
        virtual void resetDeltaTime() = 0;
        virtual float getDeltaTime() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TIME_SERVICE()\
    ((Mengine::TimeServiceInterface*)SERVICE_GET(Mengine::TimeServiceInterface))
//////////////////////////////////////////////////////////////////////////
