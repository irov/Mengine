#include "AppleAppLovinService.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleAppLovinService, Mengine::AppleAppLovinService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAppLovinService::AppleAppLovinService()
        : m_interstitialDelegate(nil)
        , m_rewardedDelegate(nil)
        , m_bannerDelegate(nil)
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

        const Char * Applovin_BannerAdUnit = CONFIG_VALUE("Applovin", "BannerAdUnit", "");

        LOGGER_INFO("applovin", "Banner AdUnit '%s'"
            , Applovin_BannerAdUnit
        );

        NSString * bannerAdUnit = [NSString stringWithUTF8String:Applovin_BannerAdUnit];

        // Banner height on iPhone and iPad is 50 and 90, respectively
        CGFloat height = (UIDevice.currentDevice.userInterfaceIdiom == UIUserInterfaceIdiomPad) ? 90 : 50;
        // Stretch to the width of the screen for banners to be fully functional
        CGFloat width = CGRectGetWidth(UIScreen.mainScreen.bounds);
        CGRect bannerRect = CGRectMake(0, 0, width, height);
        
        m_banner = [[AppLovinBannerDelegate alloc] initWithAdUnit:bannerAdUnit rect:bannerRect];

        const Char * Applovin_InterstitialAdUnit = CONFIG_VALUE("Applovin", "InterstitialAdUnit", "");

        LOGGER_INFO("applovin", "Interstitial AdUnit '%s'"
            , Applovin_InterstitialAdUnit
        );

        NSString * interstitialAdUnit = [NSString stringWithUTF8String:Applovin_InterstitialAdUnit];

        m_interstitial = [[AppLovinInterstitialDelegate alloc] initWithAdUnit:interstitialAdUnit];

        const Char * Applovin_RewardedAdUnit = CONFIG_VALUE("Applovin", "RewardedAdUnit", "");

        LOGGER_INFO("applovin", "Rewarded AdUnit '%s'"
            , Applovin_RewardedAdUnit
        );

        NSString * rewardedAdUnit = [NSString stringWithUTF8String:Applovin_RewardedAdUnit];

        m_rewarded = [[AppLovinRewardedDelegate alloc] initWithAdUnit:rewardedAdUnit callback:this];
        
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
    bool AppleAppLovinService::hasLoadedinterstitial() const
    {
        return [m_interstitial hasLoaded];
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showInterstitial()
    {
        return [m_interstitial show];
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::hasLoadedrewarded() const
    {
        return [m_rewarded hasLoaded];
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showRewarded()
    {
        return [m_rewarded show];
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::showBanner()
    {
        [m_bannerDelegate show];
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::showBanner( bool show )
    {
        [m_bannerDelegate hide];
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
