#pragma once

#include "AppleAdjustInterface.h"

#include "Kernel/ServiceBase.h"

#import "Adjust.h"

namespace Mengine
{
    class AppleAdjustService
        : public ServiceBase<AppleAdjustServiceInterface>
    {
    public:
        AppleAdjustService();
        ~AppleAdjustService();

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:

        void eventTraking(const ConstString& token) const override;
        void revenueTracking(const ConstString& token,double amount,const ConstString& currency /*= "EUR"*/) const override;

    };
}
