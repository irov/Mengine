#   pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Typedef.h"
#include "Config/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class WatchdogInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("Watchdog")

    public:
        virtual float watch( const String & _tag ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#	define WATCHDOG_SERVICE()\
	SERVICE_GET(Mengine::WatchdogInterface)
    //////////////////////////////////////////////////////////////////////////
#   ifdef MENGINE_MASTER_RELEASE
#		define WATCHDOG( tag ) (0.f)
#	else
#		define WATCHDOG( tag )\
	(SERVICE_EXIST(Mengine::WatchdogInterface) ? WATCHDOG_SERVICE()->watch(tag) : 0.f)
#	endif
}