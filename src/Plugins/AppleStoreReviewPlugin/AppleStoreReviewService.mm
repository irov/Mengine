#include "AppleStoreReviewService.h"

#include "Environment/Apple/AppleUtils.h"

#include "Kernel/Logger.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

////////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleStoreReviewService, Mengine::AppleStoreReviewService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleStoreReviewService::AppleStoreReviewService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreReviewService::~AppleStoreReviewService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreReviewService::launchTheInAppReview()
    {
        if (@available(iOS 14.0, *)) {
            UIWindow * appWindow = [UIApplication sharedApplication].delegate.window;
            [SKStoreReviewController requestReviewInScene:appWindow.windowScene];
        } else if (@available(iOS 10.3, *)) {
            [SKStoreReviewController requestReview];
        }
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleStoreReviewService::_initializeService()
    {
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleStoreReviewService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
