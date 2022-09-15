#pragma once

#include "AppleStoreReviewInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"


namespace Mengine
{
    class AppleStoreReviewService
         : public ServiceBase<AppleStoreReviewServiceInterface>
    {
    public:
        AppleStoreReviewService();
        ~AppleStoreReviewService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        virtual void launchTheInAppReview() override;

    };
}
