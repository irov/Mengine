#pragma once

#include "AppleReviewInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"


namespace Mengine
{
    class AppleReviewService
         : public ServiceBase<AppleReviewServiceInterface>
    {
    public:
        AppleReviewService();
        ~AppleReviewService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        virtual void launchTheInAppReview() override;

    };
}
