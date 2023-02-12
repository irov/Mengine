#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Lambda.h"

namespace Mengine
{
    class AppleSKAdNetworkServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleSKAdNetworkService" )

    public:
        typedef Lambda<void(bool _successful)> LambdaConversionValue;
        virtual void updateConversionValue( int64_t _value, const LambdaConversionValue & _lambda ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_SKADNETWORK_SERVICE()\
    ((Mengine::AppleSKAdNetworkServiceInterface *)SERVICE_GET(Mengine::AppleSKAdNetworkServiceInterface))
//////////////////////////////////////////////////////////////////////////
