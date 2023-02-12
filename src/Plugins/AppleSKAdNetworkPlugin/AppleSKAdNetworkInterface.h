#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class AppleSKAdNetworkServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleSKAdNetworkService" )

    public:
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_SKADNETWORK_SERVICE()\
    ((Mengine::AppleSKAdNetworkServiceInterface *)SERVICE_GET(Mengine::AppleSKAdNetworkServiceInterface))
//////////////////////////////////////////////////////////////////////////
