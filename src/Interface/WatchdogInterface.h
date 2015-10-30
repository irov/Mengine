#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Logger/Logger.h"

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
#		define BEGIN_WATCHDOG( serviceProvider, tag )
#		define END_WATCHDOG2(...) 
#		define END_WATCHDOG( serviceProvider, tag, level ) END_WATCHDOG2
#	else
#		define WATCHDOG( serviceProvider, tag ) WATCHDOG_SERVICE(serviceProvider)->watch(tag)
#		define BEGIN_WATCHDOG( serviceProvider, tag ) WATCHDOG(serviceProvider, tag)
#		define END_WATCHDOG( serviceProvider, tag, level ) if( LOGGER_SERVICE(serviceProvider)->validMessage(Menge::LM_ERROR, level) == false){}else Menge::LoggerOperator(LOGGER_SERVICE(serviceProvider), Menge::LM_ERROR, level)("(%.4f)", WATCHDOG(m_serviceProvider, tag))
#	endif
}