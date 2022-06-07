#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class AppleReviewServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleReviewService" )
            
    public:
        virtual void launchTheInAppReview() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_REVIEW_SERVICE()\
    ((Mengine::AppleReviewServiceInterface *)SERVICE_GET(Mengine::AppleReviewServiceInterface))
//////////////////////////////////////////////////////////////////////////
