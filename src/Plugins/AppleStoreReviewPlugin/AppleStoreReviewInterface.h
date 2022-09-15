#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class AppleStoreReviewServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleStoreReviewService" )
            
    public:
        virtual void launchTheInAppReview() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_STOREREVIEW_SERVICE()\
    ((Mengine::AppleStoreReviewServiceInterface *)SERVICE_GET(Mengine::AppleStoreReviewServiceInterface))
//////////////////////////////////////////////////////////////////////////
