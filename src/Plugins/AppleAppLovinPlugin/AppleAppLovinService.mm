#include "AppleAppLovinService.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleAppLovinService, Mengine::AppleAppLovinService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAppLovinService::AppleAppLovinService()
        : m_banner(nil)
        , m_interstitial(nil)
        , m_rewarded(nil)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppLovinService::~AppleAppLovinService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::_initializeService()
    {
        [ALSdk shared].settings.isVerboseLogging = YES;

        [ALSdk shared].mediationProvider = @"max";
    
        [[ALSdk shared] initializeSdkWithCompletionHandler:^(ALSdkConfiguration *configuration) {
            LOGGER_MESSAGE("AppLovin initialize");
        }];

        const Char * AppLovin_BannerAdUnit = CONFIG_VALUE("AppLovin", "BannerAdUnit", "");

        LOGGER_INFO("applovin", "Banner AdUnit '%s'"
            , AppLovin_BannerAdUnit
        );

        NSString * bannerAdUnit = [NSString stringWithUTF8String:AppLovin_BannerAdUnit];

        // Banner height on iPhone and iPad is 50 and 90, respectively
        CGFloat height = (UIDevice.currentDevice.userInterfaceIdiom == UIUserInterfaceIdiomPad) ? 90 : 50;
        // Stretch to the width of the screen for banners to be fully functional
        CGFloat width = CGRectGetWidth(UIScreen.mainScreen.bounds);
        CGRect bannerRect = CGRectMake(0, 0, width, height);
        
        m_banner = [[AppleAppLovinBannerDelegate alloc] initWithAdUnitIdentifier:bannerAdUnit rect:bannerRect];

        const Char * AppLovin_InterstitialAdUnit = CONFIG_VALUE("AppLovin", "InterstitialAdUnit", "");

        LOGGER_INFO("applovin", "Interstitial AdUnit '%s'"
            , AppLovin_InterstitialAdUnit
        );

        NSString * interstitialAdUnit = [NSString stringWithUTF8String:AppLovin_InterstitialAdUnit];

        m_interstitial = [[AppleAppLovinInterstitialDelegate alloc] initWithAdUnitIdentifier:interstitialAdUnit];

        const Char * AppLovin_RewardedAdUnit = CONFIG_VALUE("AppLovin", "RewardedAdUnit", "");

        LOGGER_INFO("applovin", "Rewarded AdUnit '%s'"
            , AppLovin_RewardedAdUnit
        );

        NSString * rewardedAdUnit = [NSString stringWithUTF8String:AppLovin_RewardedAdUnit];

        m_rewarded = [[AppleAppLovinRewardedDelegate alloc] initWithAdUnitIdentifier:rewardedAdUnit rewardCallback:this];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::_finalizeService()
    {
        [m_rewarded release];
        m_rewarded = nil;
        
        [m_interstitial release];
        m_interstitial = nil;
        
        [m_banner release];
        m_banner = nil;
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
