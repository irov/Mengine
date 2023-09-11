#include "AppleAppLovinService.h"

#include "Environment/Apple/AppleString.h"

#ifdef MENGINE_PLUGIN_APPLE_APPTRACKING
#   include "Plugins/AppleAppTrackingPlugin/AppleAppTrackingInterface.h"
#endif

#ifdef MENGINE_PLUGIN_APPLE_FIREBASE_ANALYTICS
#   include "AppleAppLovinFirebaseAnalytics.h"
#endif

#include "Kernel/Assertion.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
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
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
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
        
        bool OPTION_applovinverbose = HAS_OPTION( "applovinverbose" );
        
        if( OPTION_applovinverbose == true )
        {
            [ALSdk shared].settings.verboseLoggingEnabled = YES;
        }

        [ALSdk shared].mediationProvider = @"max";
        
        bool AppLovinPlugin_IsAgeRestrictedUser = CONFIG_VALUE( "AppLovinPlugin", "IsAgeRestrictedUser", false );
        
        if( AppLovinPlugin_IsAgeRestrictedUser == true )
        {
            [ALPrivacySettings setIsAgeRestrictedUser: YES];
        }
        else
        {
            [ALPrivacySettings setIsAgeRestrictedUser: NO];
        }
        
        bool AppLovinPlugin_CCPA = CONFIG_VALUE( "AppLovinPlugin", "CCPA", true );
        
        if( AppLovinPlugin_CCPA == true )
        {
            [ALPrivacySettings setDoNotSell: YES];
        }
        else
        {
            [ALPrivacySettings setDoNotSell: NO];
        }
        
#ifdef MENGINE_PLUGIN_APPLE_APPTRACKING
        if( APPLE_APPTRACKING_SERVICE()
           ->isTrackingAllowed() == true )
        {
            [ALPrivacySettings setHasUserConsent: YES];
        }
        else
        {
            [ALPrivacySettings setHasUserConsent: NO];
        }
#else
        [ALPrivacySettings setHasUserConsent: YES];
#endif
    
        [[ALSdk shared] initializeSdkWithCompletionHandler:^(ALSdkConfiguration *configuration) {
            LOGGER_MESSAGE("AppLovin initialize");
        }];
        
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
        m_amazonService = [[AppleAppLovinAmazonService alloc] init];
#endif
        
        m_analyticsService = [[AppleAppLovinAnalyticsService alloc] init];
        
#ifdef MENGINE_PLUGIN_APPLE_FIREBASE_ANALYTICS
        [m_analyticsService addAnalyticsDelegate:[[AppleAppLovinFirebaseAnalytics alloc] init]];
#endif
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::_finalizeService()
    {
        m_analyticsService = nil;
                
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
        
        NSString * adUnitId = Helper::stringToNSString(_adUnitId);
        NSString * placement = Helper::stringToNSString(_placement);
        
        // Banner height on iPhone and iPad is 50 and 90, respectively
        CGFloat height = (UIDevice.currentDevice.userInterfaceIdiom == UIUserInterfaceIdiomPad) ? 90 : 50;
        // Stretch to the width of the screen for banners to be fully functional
        CGFloat width = CGRectGetWidth(UIScreen.mainScreen.bounds);
        CGRect bannerRect = CGRectMake(0, 0, width, height);

        NSString * amazonBannerSlotId = nil;
        
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
        amazonBannerSlotId = [m_amazonService getAmazonBannerSlotId];
#endif
        
        AppleAppLovinBannerDelegate * banner = [[AppleAppLovinBannerDelegate alloc] initWithAdUnitIdentifier:adUnitId
                                                                                                   placement:placement
                                                                                                amazonSlotId:amazonBannerSlotId
                                                                                                        rect:bannerRect
                                                                                                    provider:_provider
                                                                                                   analytics:m_analyticsService];
        
        [m_banners setValue:banner forKey:adUnitId];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showBanner( const ConstString & _adUnitId )
    {
        NSString * adUnit = Helper::stringToNSString(_adUnitId);
        
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
        NSString * adUnit = Helper::stringToNSString(_adUnitId);
        
        AppleAppLovinBannerDelegate * banner = [m_banners objectForKey:adUnit];
        
        if( banner == nil )
        {
            return false;
        }
        
        [banner hide];
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::initInterstitial( const ConstString & _adUnitId, const AppleAppLovinInterstitialProviderInterfacePtr & _provider )
    {
        LOGGER_MESSAGE( "init interstitial AdUnit '%s'"
            , _adUnitId.c_str()
        );
        
        NSString * adUnitId = Helper::stringToNSString(_adUnitId);
        
        NSString * amazonInterstitialSlotId = nil;
        
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
        amazonInterstitialSlotId = [m_amazonService getAmazonInterstitialSlotId];
#endif

        AppleAppLovinInterstitialDelegate * interstitial = [[AppleAppLovinInterstitialDelegate alloc] initWithAdUnitIdentifier:adUnitId
                                                                                                                  amazonSlotId:amazonInterstitialSlotId
                                                                                                                      provider:_provider
                                                                                                                     analytics:m_analyticsService];
        
        [m_interstitials setValue:interstitial forKey:adUnitId];
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::canYouShowInterstitial( const ConstString & _adUnitId, const ConstString & _placement ) const
    {
        NSString * adUnit = Helper::stringToNSString(_adUnitId);
        
        AppleAppLovinInterstitialDelegate * interstitial = [m_interstitials objectForKey:adUnit];
        
        if( interstitial == nil )
        {
            return false;
        }
        
        NSString * placement = Helper::stringToNSString(_placement);
        
        BOOL result = [interstitial canYouShow:placement];
        
        return result;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showInterstitial( const ConstString & _adUnitId, const ConstString & _placement )
    {
        NSString * adUnit = Helper::stringToNSString(_adUnitId);
        
        AppleAppLovinInterstitialDelegate * interstitial = [m_interstitials objectForKey:adUnit];
        
        if( interstitial == nil )
        {
            return false;
        }
        
        NSString * placement = Helper::stringToNSString(_placement);
        
        BOOL result = [interstitial show:placement];
        
        return result;
    }
    /////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::initRewarded( const ConstString & _adUnitId, const AppleAppLovinRewardedProviderInterfacePtr & _provider )
    {
        LOGGER_MESSAGE( "init rewarded AdUnit '%s'"
            , _adUnitId.c_str()
        );
        
        NSString * adUnitId = Helper::stringToNSString(_adUnitId);
        
        NSString * amazonRewardedSlotId = nil;
        
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
        amazonRewardedSlotId = [m_amazonService getAmazonRewardedSlotId];
#endif

        AppleAppLovinRewardedDelegate * rewarded = [[AppleAppLovinRewardedDelegate alloc] initWithAdUnitIdentifier:adUnitId
                                                                                                      amazonSlotId:amazonRewardedSlotId
                                                                                                          provider:_provider
                                                                                                         analytics:m_analyticsService];
        
        [m_rewardeds setValue:rewarded forKey:adUnitId];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::canOfferRewarded( const ConstString & _adUnitId, const ConstString & _placement ) const
    {
        NSString * adUnit = Helper::stringToNSString(_adUnitId);
        
        AppleAppLovinRewardedDelegate * rewarded = [m_rewardeds objectForKey:adUnit];
        
        if( rewarded == nil )
        {
            return false;
        }
        
        NSString * placement = Helper::stringToNSString(_placement);
        
        BOOL result = [rewarded canOffer:placement];
        
        return result;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::canYouShowRewarded( const ConstString & _adUnitId, const ConstString & _placement ) const
    {
        NSString * adUnit = Helper::stringToNSString(_adUnitId);
        
        AppleAppLovinRewardedDelegate * rewarded = [m_rewardeds objectForKey:adUnit];
        
        if( rewarded == nil )
        {
            return false;
        }
        
        NSString * placement = Helper::stringToNSString(_placement);
        
        BOOL result = [rewarded canYouShow:placement];
        
        return result;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showRewarded( const ConstString & _adUnitId, const ConstString & _placement )
    {
        NSString * adUnit = Helper::stringToNSString(_adUnitId);
        
        AppleAppLovinRewardedDelegate * rewarded = [m_rewardeds objectForKey:adUnit];
        
        if( rewarded == nil )
        {
            return false;
        }
        
        NSString * placement = Helper::stringToNSString(_placement);
        
        BOOL result = [rewarded show:placement];
        
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::showMediationDebugger()
    {
        [[ALSdk shared] showMediationDebugger];
    }
    //////////////////////////////////////////////////////////////////////////
}
