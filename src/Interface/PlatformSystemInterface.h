#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class PlatformSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PlatformSystem" );
    };
}
//////////////////////////////////////////////////////////////////////////
#define PLATFORM_SYSTEM()\
    ((Mengine::PlatformSystemInterface *)SERVICE_GET(Mengine::PlatformSystemInterface))
//////////////////////////////////////////////////////////////////////////