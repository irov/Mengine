#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Logger/Logger.h"

#   include "Config/Typedef.h"

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

#   ifdef MENGE_MASTER_RELEASE
#   define WATCHDOG( serviceProvider, tag )\
    (0.f)
#   else //MENGE_MASTER_RELEASE
#   define WATCHDOG( serviceProvider, tag )\
    WATCHDOG_SERVICE(serviceProvider)->watch(tag)
#   endif //MENGE_MASTER_RELEASE

#   define LOGGER_WATCHDOG( serviceProvider, level )\
    if( LOG_SERVICE(serviceProvider)->validMessage(Menge::LM_ERROR, level) == false) {} else Menge::LoggerOperator(LOG_SERVICE(serviceProvider), Menge::LM_ERROR, level)

#   define BEGIN_WATCHDOG( serviceProvider, tag )\
    WATCHDOG(serviceProvider, tag)

#   define END_WATCHDOG( serviceProvider, tag, level )\
    LOGGER_WATCHDOG(serviceProvider, level)("(%.4f)", WATCHDOG(m_serviceProvider, tag))
}