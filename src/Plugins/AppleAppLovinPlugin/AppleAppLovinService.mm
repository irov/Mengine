#include "AppleAppLovinService.h"

#import "Environment/Apple/AppleString.h"

#if defined(MENGINE_PLUGIN_APPLE_APPTRACKING)
#   include "Plugins/AppleAppTrackingPlugin/AppleAppTrackingInterface.h"
#endif

#include "AppleAppLovinApplicationDelegate.h"

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
        : m_banners( nil )
        , m_interstitials( nil )
        , m_rewardeds( nil )
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
        , m_amazonService( nil )
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
        m_banners = [[NSMutableDictionary alloc] init];
        m_interstitials = [[NSMutableDictionary alloc] init];
        m_rewardeds = [[NSMutableDictionary alloc] init];
        
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
                
        m_banners = nil;
        m_interstitials = nil;
        m_rewardeds = nil;
    }
    /////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::initBanner( const ConstString & _adUnitId, const ConstString & _placement, const AppleAppLovinBannerProviderInterfacePtr & _provider )
    {
        LOGGER_MESSAGE( "init banner AdUnit '%s'"
            , _adUnitId.c_str()
        );
        
        NSString * adUnitId = [AppleString NSStringFromConstString:_adUnitId];
        NSString * placement = [AppleString NSStringFromConstString:_placement];
        
        // Banner height on iPhone and iPad is 50 and 90, respectively
        CGFloat banner_height = (UIDevice.currentDevice.userInterfaceIdiom == UIUserInterfaceIdiomPad) ? 90 : 50;
        // Stretch to the width of the screen for banners to be fully functional
        CGFloat screen_width = CGRectGetWidth(UIScreen.mainScreen.bounds);
        CGFloat screen_height = CGRectGetHeight(UIScreen.mainScreen.bounds);
        CGRect bannerRect = CGRectMake(0, screen_height - banner_height, screen_width, banner_height);

        NSString * amazonBannerSlotId = nil;
        
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
        amazonBannerSlotId = [m_amazonService getAmazonBannerSlotId];
#endif
        
        AppleAppLovinBannerDelegate * banner = [[AppleAppLovinBannerDelegate alloc] initWithAdUnitIdentifier:adUnitId
                                                                                                   placement:placement
                                                                                                amazonSlotId:amazonBannerSlotId
                                                                                                        rect:bannerRect
                                                                                                    provider:_provider];
        
        if( banner == nil )
        {
            return false;
        }
        
        [m_banners setValue:banner forKey:adUnitId];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showBanner( const ConstString & _adUnitId )
    {
        NSString * adUnit = [AppleString NSStringFromConstString:_adUnitId];
        
        AppleAppLovinBannerDelegate * banner = [m_banners objectForKey:adUnit];
        
        if( banner == nil )
        {
            return false;
        }
        
        [banner show];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::hideBanner( const ConstString & _adUnitId )
    {
        NSString * adUnit = [AppleString NSStringFromConstString:_adUnitId];
        
        AppleAppLovinBannerDelegate * banner = [m_banners objectForKey:adUnit];
        
        if( banner == nil )
        {
            return false;
        }
        
        [banner hide];
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::getBannerViewport( const ConstString & _adUnitId, Viewport * const _viewport ) const
    {
        NSString * adUnit = [AppleString NSStringFromConstString:_adUnitId];
        
        AppleAppLovinBannerDelegate * banner = [m_banners objectForKey:adUnit];
        
        if( banner == nil )
        {
            return false;
        }
        
        CGRect rect = [banner getRect];
        
        _viewport->begin.x = rect.origin.x;
        _viewport->begin.y = rect.origin.y;
        
        _viewport->end.x = rect.origin.x + rect.size.width;
        _viewport->end.y = rect.origin.y + rect.size.height;
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::initInterstitial( const ConstString & _adUnitId, const AppleAppLovinInterstitialProviderInterfacePtr & _provider )
    {
        LOGGER_MESSAGE( "init interstitial AdUnit '%s'"
            , _adUnitId.c_str()
        );
        
        NSString * adUnitId = [AppleString NSStringFromConstString:_adUnitId];
        
        NSString * amazonInterstitialSlotId = nil;
        
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
        amazonInterstitialSlotId = [m_amazonService getAmazonInterstitialSlotId];
#endif

        AppleAppLovinInterstitialDelegate * interstitial = [[AppleAppLovinInterstitialDelegate alloc] initWithAdUnitIdentifier:adUnitId
                                                                                                                  amazonSlotId:amazonInterstitialSlotId
                                                                                                                      provider:_provider];
        
        if( interstitial == nil )
        {
            return false;
        }
        
        [m_interstitials setValue:interstitial forKey:adUnitId];
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::canYouShowInterstitial( const ConstString & _adUnitId, const ConstString & _placement ) const
    {
        NSString * adUnit = [AppleString NSStringFromConstString:_adUnitId];
        
        AppleAppLovinInterstitialDelegate * interstitial = [m_interstitials objectForKey:adUnit];
        
        if( interstitial == nil )
        {
            return false;
        }
        
        NSString * placement = [AppleString NSStringFromConstString:_placement];
        
        BOOL result = [interstitial canYouShow:placement];
        
        return result;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showInterstitial( const ConstString & _adUnitId, const ConstString & _placement )
    {
        NSString * adUnit = [AppleString NSStringFromConstString:_adUnitId];
        
        AppleAppLovinInterstitialDelegate * interstitial = [m_interstitials objectForKey:adUnit];
        
        if( interstitial == nil )
        {
            return false;
        }
        
        NSString * placement = [AppleString NSStringFromConstString:_placement];
        
        BOOL result = [interstitial show:placement];
        
        return result;
    }
    /////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::initRewarded( const ConstString & _adUnitId, const AppleAppLovinRewardedProviderInterfacePtr & _provider )
    {
        LOGGER_MESSAGE( "init rewarded AdUnit '%s'"
            , _adUnitId.c_str()
        );
        
        NSString * adUnitId = [AppleString NSStringFromConstString:_adUnitId];
        
        NSString * amazonRewardedSlotId = nil;
        
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
        amazonRewardedSlotId = [m_amazonService getAmazonRewardedSlotId];
#endif

        AppleAppLovinRewardedDelegate * rewarded = [[AppleAppLovinRewardedDelegate alloc] initWithAdUnitIdentifier:adUnitId
                                                                                                      amazonSlotId:amazonRewardedSlotId
                                                                                                          provider:_provider];
        
        if( rewarded == nil )
        {
            return false;
        }
        
        [m_rewardeds setValue:rewarded forKey:adUnitId];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::canOfferRewarded( const ConstString & _adUnitId, const ConstString & _placement ) const
    {
        NSString * adUnit = [AppleString NSStringFromConstString:_adUnitId];
        
        AppleAppLovinRewardedDelegate * rewarded = [m_rewardeds objectForKey:adUnit];
        
        if( rewarded == nil )
        {
            return false;
        }
        
        NSString * placement = [AppleString NSStringFromConstString:_placement];
        
        BOOL result = [rewarded canOffer:placement];
        
        return result;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::canYouShowRewarded( const ConstString & _adUnitId, const ConstString & _placement ) const
    {
        NSString * adUnit = [AppleString NSStringFromConstString:_adUnitId];
        
        AppleAppLovinRewardedDelegate * rewarded = [m_rewardeds objectForKey:adUnit];
        
        if( rewarded == nil )
        {
            return false;
        }
        
        NSString * placement = [AppleString NSStringFromConstString:_placement];
        
        BOOL result = [rewarded canYouShow:placement];
        
        return result;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showRewarded( const ConstString & _adUnitId, const ConstString & _placement )
    {
        NSString * adUnit = [AppleString NSStringFromConstString:_adUnitId];
        
        AppleAppLovinRewardedDelegate * rewarded = [m_rewardeds objectForKey:adUnit];
        
        if( rewarded == nil )
        {
            return false;
        }
        
        NSString * placement = [AppleString NSStringFromConstString:_placement];
        
        BOOL result = [rewarded show:placement];
        
        return result;
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
        
        return configuration.consentFlowUserGeography == ALConsentFlowUserGeographyGDPR;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::loadAndShowCMPFlow( const AppleAppLovinConsentFlowProviderInterfacePtr & _provider )
    {
        ALCMPService * cmpService = [ALSdk shared].cmpService;
        
        AppleAppLovinConsentFlowProviderInterfacePtr copy_provider = _provider;

        [cmpService showCMPForExistingUserWithCompletion:^(ALCMPError * _Nullable error) {
            if (error != nil)
            {
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
