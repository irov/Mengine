#pragma once

#include "Interface/WatchdogInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Typedef.h"
#include "Config/Map.h"

namespace Mengine
{
    class WatchdogService
        : public ServiceBase<WatchdogInterface>
    {
    public:
        WatchdogService();
        ~WatchdogService() override;

    public:
        double watch( const String & _tag ) override;

    protected:
        typedef Map<String, uint64_t> MapWatchers;
        MapWatchers m_watchers;
    };
}