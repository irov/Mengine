#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class DevToDebugServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "DevToDebugService" )

    public:
    };
}
//////////////////////////////////////////////////////////////////////////
#define DEVTODEBUG_SERVICE()\
    ((Mengine::DevToDebugServiceInterface *)SERVICE_GET(Mengine::DevToDebugServiceInterface))
//////////////////////////////////////////////////////////////////////////