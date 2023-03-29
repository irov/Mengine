#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Typedef.h"

namespace Mengine
{
    class WatchdogServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "WatchdogService" )

    public:
        virtual double watch( const ConstString & _tag ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define WATCHDOG_SERVICE()\
    ((Mengine::WatchdogServiceInterface*)SERVICE_GET(Mengine::WatchdogServiceInterface))
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_MASTER_RELEASE)
#   define MENGINE_WATCHDOG( tag ) (0.0)
#else
#   define MENGINE_WATCHDOG( tag ) (SERVICE_IS_INITIALIZE(Mengine::WatchdogServiceInterface) ? WATCHDOG_SERVICE()->watch(tag) : 0.0)
#endif
//////////////////////////////////////////////////////////////////////////