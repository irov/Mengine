#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Config/Typedef.h"
#   include "Config/String.h"

namespace Menge
{
    class WatchdogInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("Watchdog")

    public:
        virtual float watch( const String & _tag ) = 0;
    };

#	define WATCHDOG_SERVICE( serviceProvider )\
	SERVICE_GET(serviceProvider, Menge::WatchdogInterface)


#   ifdef MENGINE_MASTER_RELEASE
#		define WATCHDOG( serviceProvider, tag ) (0.f)
#	else
#		define WATCHDOG( serviceProvider, tag )\
	(SERVICE_EXIST(serviceProvider, Menge::WatchdogInterface) ? WATCHDOG_SERVICE(serviceProvider)->watch(tag) : 0.f)
#	endif
}