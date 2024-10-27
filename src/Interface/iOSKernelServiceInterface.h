#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class iOSKernelServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "iOSKernelService" )
    };
}
//////////////////////////////////////////////////////////////////////////
#define IOS_KERNEL_SERVICE()\
    ((Mengine::iOSKernelServiceInterface *)SERVICE_GET(Mengine::iOSKernelServiceInterface))
//////////////////////////////////////////////////////////////////////////
