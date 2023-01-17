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
    /////////////////////////////////////////////////////////////////////////////
    bool AppleStoreReviewService::_initializeService()
    {
        //Empty
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleStoreReviewService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreReviewService::launchTheInAppReview()
    {
        LOGGER_INFO("storereview", "launch the InAppReview");
        
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
    //////////////////////////////////////////////////////////////////////////
}
