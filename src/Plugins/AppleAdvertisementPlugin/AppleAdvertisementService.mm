#include "AppleAppLovinService.h"

#import "Environment/Apple/AppleString.h"
#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSNetwork.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_APPLE_APPTRACKING)
#   include "Plugins/AppleAppTrackingPlugin/AppleAppTrackingInterface.h"
#endif

#import "AppleAppLovinApplicationDelegate.h"

#include "Kernel/Assertion.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleAppLovinService, Mengine::AppleAppLovinService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAppLovinService::AppleAppLovinService()
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
        : m_amazonService( nil )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppLovinService::~AppleAppLovinService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::_initializeService()
    {
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
        m_amazonService = [[AppleAppLovinAmazonService alloc] init];
#endif
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::_finalizeService()
    {
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
        m_amazonService = nil;
#endif
    }
    /////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::setBannerProvider( const AppleAppLovinBannerProviderInterfacePtr & _bannerProvider )
    {
        [[AppleAppLovinApplicationDelegate sharedInstance] setBannerProvider:_bannerProvider];
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showBanner()
    {
        AppleAppLovinBannerDelegate * banner = [[AppleAppLovinApplicationDelegate sharedInstance] getBanner];
        
        if( banner == nil )
        {
            return false;
        }
        
        [banner show];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::hideBanner()
    {
        AppleAppLovinBannerDelegate * banner = [[AppleAppLovinApplicationDelegate sharedInstance] getBanner];
        
        if( banner == nil )
        {
            return false;
        }
        
        [banner hide];
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::getBannerHeight( uint32_t * const _height ) const
    {
        AppleAppLovinBannerDelegate * banner = [[AppleAppLovinApplicationDelegate sharedInstance] getBanner];
        
        if( banner == nil )
        {
            return false;
        }
        
        CGFloat banner_height = [banner getHeight];
        
        *_height = (uint32_t)banner_height;
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::setInterstitialProvider( const AppleAppLovinInterstitialProviderInterfacePtr & _interstitialProvider )
    {
        [[AppleAppLovinApplicationDelegate sharedInstance] setInterstitialProvider:_interstitialProvider];
    }
    /////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::canYouShowInterstitial( const ConstString & _placement ) const
    {
        AppleAppLovinInterstitialDelegate * interstitial = [[AppleAppLovinApplicationDelegate sharedInstance] getInterstitial];
        
        if( interstitial == nil )
        {
            return false;
        }
        
        if( [[iOSNetwork sharedInstance] isNetworkAvailable] == NO )
        {
            return false;
        }
        
        NSString * placement = [AppleString NSStringFromConstString:_placement];
        
        if( [interstitial canYouShow:placement] == NO )
        {
            return false;
        }
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showInterstitial( const ConstString & _placement )
    {
        AppleAppLovinInterstitialDelegate * interstitial = [[AppleAppLovinApplicationDelegate sharedInstance] getInterstitial];
        
        if( interstitial == nil )
        {
            return false;
        }
        
        if( [[iOSNetwork sharedInstance] isNetworkAvailable] == NO )
        {
            return false;
        }
        
        NSString * placement = [AppleString NSStringFromConstString:_placement];
        
        if( [interstitial show:placement] == NO )
        {
            return false;
        }
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::canOfferRewarded( const ConstString & _placement ) const
    {
        AppleAppLovinRewardedDelegate * rewarded = [[AppleAppLovinApplicationDelegate sharedInstance] getRewarded];
        
        if( rewarded == nil )
        {
            return false;
        }
        
        if( [[iOSNetwork sharedInstance] isNetworkAvailable] == NO )
        {
            return false;
        }
        
        NSString * placement = [AppleString NSStringFromConstString:_placement];
        
        if( [rewarded canOffer:placement] == NO )
        {
            return false;
        }
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::canYouShowRewarded( const ConstString & _placement ) const
    {
        AppleAppLovinRewardedDelegate * rewarded = [[AppleAppLovinApplicationDelegate sharedInstance] getRewarded];
        
        if( rewarded == nil )
        {
            return false;
        }
        
        if( [[iOSNetwork sharedInstance] isNetworkAvailable] == NO )
        {
            return false;
        }
        
        NSString * placement = [AppleString NSStringFromConstString:_placement];
        
        if( [rewarded canYouShow:placement] == NO )
        {
            return false;
        }
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::setRewardedProvider( const AppleAppLovinRewardedProviderInterfacePtr & _rewardedProvider )
    {
        [[AppleAppLovinApplicationDelegate sharedInstance] setRewardedProvider:_rewardedProvider];
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showRewarded( const ConstString & _placement )
    {
        AppleAppLovinRewardedDelegate * rewarded = [[AppleAppLovinApplicationDelegate sharedInstance] getRewarded];
        
        if( rewarded == nil )
        {
            return false;
        }
        
        if( [[iOSNetwork sharedInstance] isNetworkAvailable] == NO )
        {
            return false;
        }
        
        NSString * placement = [AppleString NSStringFromConstString:_placement];
        
        if( [rewarded show:placement] == NO )
        {
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::hasSupportedCMP() const
    {
        ALCMPService * cmpService = [ALSdk shared].cmpService;
        
        if( [cmpService hasSupportedCMP] == NO )
        {
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::isConsentFlowUserGeographyGDPR() const
    {
        ALSdkConfiguration * configuration = [ALSdk shared].configuration;
        
        if( configuration.consentFlowUserGeography != ALConsentFlowUserGeographyGDPR )
        {
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::loadAndShowCMPFlow( const AppleAppLovinConsentFlowProviderInterfacePtr & _provider )
    {
        ALCMPService * cmpService = [ALSdk shared].cmpService;
        
        AppleAppLovinConsentFlowProviderInterfacePtr copy_provider = _provider;

        [cmpService showCMPForExistingUserWithCompletion:^(ALCMPError * _Nullable error) {
            if (error != nil) {
                LOGGER_ERROR( "AppLovin CMP show failed: %s [%ld] message: %s"
                    , [error.message UTF8String]
                    , error.code
                    , [error.cmpMessage UTF8String]
                );
                
                Mengine::Helper::dispatchMainThreadEvent([copy_provider]() {
                    copy_provider->onAppleAppLovinConsentFlowShowFailed();
                });
                
                return;
            }
            
            LOGGER_MESSAGE( "AppLovin CMP show successful" );
            
            Mengine::Helper::dispatchMainThreadEvent([copy_provider]() {
                copy_provider->onAppleAppLovinConsentFlowShowSuccessful();
            });
        }];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::showMediationDebugger()
    {
        [[ALSdk shared] showMediationDebugger];
    }
    //////////////////////////////////////////////////////////////////////////
}
