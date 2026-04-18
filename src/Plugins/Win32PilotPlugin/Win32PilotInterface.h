#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class Win32PilotServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "Win32PilotService" )
    };
}
//////////////////////////////////////////////////////////////////////////
#define WIN32_PILOT_SERVICE()\
    ((Mengine::Win32PilotServiceInterface *)SERVICE_GET(Mengine::Win32PilotServiceInterface))
//////////////////////////////////////////////////////////////////////////
