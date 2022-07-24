#include "AppleAppLovinService.h"

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
        : m_banner( nil )
        , m_interstitial( nil )
        , m_rewarded( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppLovinService::~AppleAppLovinService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::_initializeService()
    {
        bool OPTION_applovinverbose = HAS_OPTION("applovinverbose");
        bool AppLovin_VerboseLogging = CONFIG_VALUE("AppLovin", "VerboseLogging", false);
        
        if( OPTION_applovinverbose == true || AppLovin_VerboseLogging == true )
        {
            [ALSdk shared].settings.isVerboseLogging = YES;
        }

        [ALSdk shared].mediationProvider = @"max";
    
        [[ALSdk shared] initializeSdkWithCompletionHandler:^(ALSdkConfiguration *configuration) {
            LOGGER_MESSAGE("AppLovin initialize");
        }];
        
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
        m_amazonService = [[AppleAppLovinAmazonService alloc] init];
#endif
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::_finalizeService()
    {
        if( m_rewarded != nil )
        {
            [m_rewarded release];
            m_rewarded = nil;
        }
        
        if( m_interstitial != nil )
        {
            [m_interstitial release];
            m_interstitial = nil;
        }
        
        if( m_banner != nil )
        {
            [m_banner release];
            m_banner = nil;
        }
    }
    /////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::initBanner()
    {
        const Char * AppLovin_BannerAdUnit = CONFIG_VALUE("AppLovin", "BannerAdUnit", "");

        MENGINE_ASSERTION_FATAL( MENGINE_STRCMP( AppLovin_BannerAdUnit, "" ) == 0 );

        LOGGER_INFO("applovin", "Banner AdUnit '%s'"
            , AppLovin_BannerAdUnit
        );

        NSString * bannerAdUnit = [NSString stringWithUTF8String:AppLovin_BannerAdUnit];

        // Banner height on iPhone and iPad is 50 and 90, respectively
        CGFloat height = (UIDevice.currentDevice.userInterfaceIdiom == UIUserInterfaceIdiomPad) ? 90 : 50;
        // Stretch to the width of the screen for banners to be fully functional
        CGFloat width = CGRectGetWidth(UIScreen.mainScreen.bounds);
        CGRect bannerRect = CGRectMake(0, 0, width, height);

        NSString * amazonBannerSlotId = nil;
        
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
        amazonBannerSlotId = [m_amazonService getAmazonBannerSlotId];
#endif
        
        m_banner = [[AppleAppLovinBannerDelegate alloc] initWithAdUnitIdentifier:bannerAdUnit amazonBannerSlotId:amazonBannerSlotId rect:bannerRect];
    }
    /////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::initInterstitial()
    {
        const Char * AppLovin_InterstitialAdUnit = CONFIG_VALUE("AppLovin", "InterstitialAdUnit", "");
        
        MENGINE_ASSERTION_FATAL( MENGINE_STRCMP( AppLovin_InterstitialAdUnit, "" ) == 0 );
        
        LOGGER_INFO("applovin", "Interstitial AdUnit '%s'"
                    , AppLovin_InterstitialAdUnit
                    );
        
        NSString * interstitialAdUnit = [NSString stringWithUTF8String:AppLovin_InterstitialAdUnit];
        
        NSString * amazonInterstitialSlotId = nil;
        
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
        amazonInterstitialSlotId = [m_amazonService getAmazonInterstitialSlotId];
#endif

        m_interstitial = [[AppleAppLovinInterstitialDelegate alloc] initWithAdUnitIdentifier:interstitialAdUnit amazonInterSlotId:amazonInterstitialSlotId];
    }
    /////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::initRewarded()
    {
        const Char * AppLovin_RewardedAdUnit = CONFIG_VALUE("AppLovin", "RewardedAdUnit", "");
        
        MENGINE_ASSERTION_FATAL( MENGINE_STRCMP( AppLovin_RewardedAdUnit, "" ) == 0 );

        LOGGER_INFO("applovin", "Rewarded AdUnit '%s'"
            , AppLovin_RewardedAdUnit
        );

        NSString * rewardedAdUnit = [NSString stringWithUTF8String:AppLovin_RewardedAdUnit];
        
        NSString * amazonRewardedSlotId = nil;
        
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
        amazonRewardedSlotId = [m_amazonService getAmazonRewardedSlotId];
#endif

        m_rewarded = [[AppleAppLovinRewardedDelegate alloc] initWithAdUnitIdentifier: rewardedAdUnit
                                                                amazonRewardedSlotId: amazonRewardedSlotId
                                                                      rewardCallback: this];
    }
    /////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::hasLoadedInterstitial() const
    {
        BOOL result = [m_interstitial hasLoaded];
        
        return result;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showInterstitial()
    {
        BOOL result = [m_interstitial show];
        
        return result;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::hasLoadedRewarded() const
    {
        BOOL result = [m_rewarded hasLoaded];
        
        return result;
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showRewarded()
    {
        BOOL result = [m_rewarded show];
        
        return result;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::showBanner()
    {
        [m_banner show];
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::hideBanner()
    {
        [m_banner hide];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::showMediationDebugger()
    {
        [[ALSdk shared] showMediationDebugger];
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::onAppLovinRewardReceivedReward( uint64_t _amount )
    {
        MENGINE_UNUSED( _amount );
    }
    //////////////////////////////////////////////////////////////////////////
}
