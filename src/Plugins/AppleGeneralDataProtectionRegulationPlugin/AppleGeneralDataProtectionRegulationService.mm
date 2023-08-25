#include "AppleGeneralDataProtectionRegulationService.h"

#include "Environment/Apple/AppleUtils.h"

#include "Kernel/Logger.h"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

////////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleGeneralDataProtectionRegulationService, Mengine::AppleGeneralDataProtectionRegulationService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleGeneralDataProtectionRegulationService::AppleGeneralDataProtectionRegulationService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGeneralDataProtectionRegulationService::~AppleGeneralDataProtectionRegulationService()
    {
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleGeneralDataProtectionRegulationService::_initializeService()
    {
        //Empty
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleGeneralDataProtectionRegulationService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGeneralDataProtectionRegulationService::setGDPRPass( bool _passGDPR )
    {
        LOGGER_MESSAGE( "set GDPR pass [%d]"
            , _passGDPR
        );
        
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
