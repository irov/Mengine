#pragma once

#include "Interface/PlatformSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class iOSPlatformSystem
        : public ServiceBase<PlatformSystemInterface>
    {
    public:
        iOSPlatformSystem();
        ~iOSPlatformSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool beginThread( ThreadId _threadId ) override;
        void endThread( ThreadId _threadId ) override;
    };
}
