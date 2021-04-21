#pragma once

#include "Interface/WatchdogServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Map.h"

#include "Config/Typedef.h"

namespace Mengine
{
    class WatchdogService
        : public ServiceBase<WatchdogServiceInterface>
    {
    public:
        WatchdogService();
        ~WatchdogService() override;

    public:
        double watch( const ConstString & _tag ) override;

    protected:
        typedef Map<ConstString, uint64_t> MapWatchers;
        MapWatchers m_watchers;
    };
}