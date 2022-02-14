#pragma once

#include "SentryInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleSentryService
        : public ServiceBase<SentryServiceInterface>
    {
    public:
        AppleSentryService();
        ~AppleSentryService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void notifyCreateApplication_();
    };
}