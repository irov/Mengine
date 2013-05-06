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
        virtual void destroy() = 0;
    };

#   ifdef MENGE_MASTER_RELEASE
#   define WATCHDOG( serviceProvider, tag )\
    (0.f)
#   else //MENGE_MASTER_RELEASE
#   define WATCHDOG( serviceProvider, tag )\
    (Menge::getService<Menge::WatchdogInterface>(serviceProvider)->watch(tag))
#   endif //MENGE_MASTER_RELEASE

#   define LOGGER_WATCHDOG( serviceProvider )\
    if( LOG_SERVICE(serviceProvider)->validMessage(Menge::LM_ERROR, 1) == false) {} else Menge::LoggerOperator(LOG_SERVICE(serviceProvider), Menge::LM_ERROR, 1)
}