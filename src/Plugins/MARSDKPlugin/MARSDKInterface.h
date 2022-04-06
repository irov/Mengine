#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class MARSDKServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "MARSDKService" )

    public:
    };
}
//////////////////////////////////////////////////////////////////////////
#define MARSDK_SERVICE()\
    ((Mengine::MARSDKServiceInterface *)SERVICE_GET(Mengine::MARSDKServiceInterface))
//////////////////////////////////////////////////////////////////////////