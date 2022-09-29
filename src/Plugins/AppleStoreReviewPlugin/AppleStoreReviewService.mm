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
            UIWindowScene * foregroundScene = nil;
            for( UIWindowScene * scene in UIApplication.sharedApplication.connectedScenes ) {
                if( scene.activationState == UISceneActivationStateForegroundActive ) {
                    foregroundScene = scene;
                }
            }
            
            if( foregroundScene != nil ) {
                [SKStoreReviewController requestReviewInScene:foregroundScene];
            }
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
