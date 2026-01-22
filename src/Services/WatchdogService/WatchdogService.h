#pragma once

#include "Interface/WatchdogServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Map.h"

#include "Config/Timestamp.h"
#include "Config/Typedef.h"

namespace Mengine
{
    class WatchdogService
        : public ServiceBase<WatchdogServiceInterface>
    {
        DECLARE_FACTORABLE( WatchdogService );

    public:
        WatchdogService();
        ~WatchdogService() override;

    public:
        double watch( const ConstString & _tag ) override;

    protected:
        typedef Map<ConstString, Timestamp> MapWatchers;
        MapWatchers m_watchers;
    };
}