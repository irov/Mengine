#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class AppleAppTrackingInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleAppTracking" )

    public:
        virtual void authorization() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_APPTRACKING_SERVICE()\
    ((Mengine::AppleAppTrackingInterface *)SERVICE_GET(Mengine::AppleAppTrackingInterface))
//////////////////////////////////////////////////////////////////////////