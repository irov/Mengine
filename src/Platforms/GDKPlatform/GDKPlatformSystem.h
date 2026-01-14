#pragma once

#include "Interface/PlatformSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class GDKPlatformSystem
        : public ServiceBase<PlatformSystemInterface>
    {
    public:
        GDKPlatformSystem();
        ~GDKPlatformSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        bool beginThread( ThreadId _threadId ) override;
        void endThread( ThreadId _threadId ) override;
    };
}
