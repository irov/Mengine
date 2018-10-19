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
        float watch( const String & _tag ) override;

    protected:
        typedef Map<String, float> MapWatchers;
        MapWatchers m_watchers;
    };
}