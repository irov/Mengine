#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class AppleKernelServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleEnvironmentService" )
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_KERNEL_SERVICE()\
    ((Mengine::AppleKernelServiceInterface *)SERVICE_GET(Mengine::AppleKernelServiceInterface))
//////////////////////////////////////////////////////////////////////////
