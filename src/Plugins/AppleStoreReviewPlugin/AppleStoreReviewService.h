#pragma once

#include "AppleStoreReviewInterface.h"

#include "Kernel/ServiceBase.h"

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
        void launchTheInAppReview() override;

    };
}
