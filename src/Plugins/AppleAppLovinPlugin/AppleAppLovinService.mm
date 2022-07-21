#include "AppleAppLovinService.h"

#include "Kernel/Assertion.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

#import <DTBiOSSDK/DTBAds.h>

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
        
        const Char * AppLovin_AmazoneAppId = CONFIG_VALUE("AppLovin", "AmazonAppId", "");

        if( MENGINE_STRCMP( AppLovin_AmazoneAppId, "" ) != 0 )
        {
            LOGGER_INFO("applovin", "Amazone AppID '%s'", AppLovin_AmazoneAppId);
            
            NSString * appId = [NSString stringWithUTF8String:AppLovin_AmazoneAppId];
            
            [[DTBAds sharedInstance] setAppKey: appId];
            DTBAdNetworkInfo *adNetworkInfo = [[DTBAdNetworkInfo alloc] initWithNetworkName: DTBADNETWORK_MAX];
            [DTBAds sharedInstance].mraidCustomVersions = @[@"1.0", @"2.0", @"3.0"];
            [[DTBAds sharedInstance] setAdNetworkInfo: adNetworkInfo];
            [DTBAds sharedInstance].mraidPolicy = CUSTOM_MRAID;
                        
//            DTBAds.sharedInstance().setLogLevel(DTBLogLevelAll)
//            DTBAds.sharedInstance().testMode = true
        }
        
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
        
        NSString * amazonBannerSlotId = NULL;
        const Char * AppLovin_amazonBannerSlotId = CONFIG_VALUE("AppLovin", "AmazonBannerSlotId", "");

        if( MENGINE_STRCMP( AppLovin_BannerAdUnit, "" ) != 0 )
        {
            amazonBannerSlotId = [NSString stringWithUTF8String:AppLovin_BannerAdUnit];
            
            LOGGER_INFO("applovin", "Interstitial Amazon AdUnit '%s'"
                , AppLovin_amazonBannerSlotId
            );
        }
        
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
        
        NSString * amazonInterSlotId = NULL;
        const Char * AppLovin_amazonInterSlotId = CONFIG_VALUE("AppLovin", "AmazonInterstitialSlodId", "");
        
        if(MENGINE_STRCMP( AppLovin_amazonInterSlotId, "" ) != 0 ){
            amazonInterSlotId = [NSString stringWithUTF8String:AppLovin_amazonInterSlotId];
            
            LOGGER_INFO("applovin", "Interstitial Amazon AdUnit '%s'"
                        , AppLovin_amazonInterSlotId
                        );
        }

        m_interstitial = [[AppleAppLovinInterstitialDelegate alloc] initWithAdUnitIdentifier:interstitialAdUnit amazonInterSlotId:amazonInterSlotId];
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
        
        NSString * amazonRewardedSlotId = NULL;
        const Char * AppLovin_amazonRewardedSlotId = CONFIG_VALUE("AppLovin", "AmazonVideoRewardedSlotId", "");
        
        if(MENGINE_STRCMP( AppLovin_amazonRewardedSlotId, "" ) != 0 ){
            amazonRewardedSlotId = [NSString stringWithUTF8String:AppLovin_amazonRewardedSlotId];
            
            LOGGER_INFO("applovin", "Rewarded Amazon AdUnit '%s'"
                        , AppLovin_amazonRewardedSlotId
                        );
        }

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
