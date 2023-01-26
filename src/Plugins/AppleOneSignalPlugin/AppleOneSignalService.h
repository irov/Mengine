#pragma once

#include "AppleOneSignalInterface.h"

#include "Kernel/ServiceBase.h"

#import <OneSignal/OneSignal.h>

namespace Mengine
{
    class AppleOneSignalService
        : public ServiceBase<AppleOneSignalServiceInterface>
    {
    public:
        AppleOneSignalService();
        ~AppleOneSignalService();

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
    };
}
