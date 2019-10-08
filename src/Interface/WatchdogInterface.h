#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Typedef.h"

namespace Mengine
{
    class WatchdogInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "Watchdog" )

    public:
        virtual double watch( const ConstString & _tag ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define WATCHDOG_SERVICE()\
    ((Mengine::WatchdogInterface*)SERVICE_GET(Mengine::WatchdogInterface))
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_MASTER_RELEASE
#   define WATCHDOG( tag ) (0.0)
#else
#   define WATCHDOG( tag ) (SERVICE_EXIST(Mengine::WatchdogInterface) ? WATCHDOG_SERVICE()->watch(tag) : 0.0)
#endif
//////////////////////////////////////////////////////////////////////////