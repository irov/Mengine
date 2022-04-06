#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class AppleMARSDKServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleMARSDKService" )

    public:
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_MARSDK_SERVICE()\
    ((Mengine::AppleMARSDKServiceInterface *)SERVICE_GET(Mengine::AppleMARSDKServiceInterface))
//////////////////////////////////////////////////////////////////////////