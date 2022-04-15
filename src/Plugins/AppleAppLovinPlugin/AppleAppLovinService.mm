#include "AppleAppLovinService.h"

#include "Kernel/Logger.h"

#include "Config/Algorithm.h"

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
        NSString* banner_key = [NSString stringWithUTF8String:"6c5e2fd2fa0e48e6"];

        // Banner height on iPhone and iPad is 50 and 90, respectively
        CGFloat height = (UIDevice.currentDevice.userInterfaceIdiom == UIUserInterfaceIdiomPad) ? 90 : 50;
        // Stretch to the width of the screen for banners to be fully functional
        CGFloat width = CGRectGetWidth(UIScreen.mainScreen.bounds);
        CGRect bannerRect = CGRectMake(0, 0, width, height);
        
        m_bannerDelegate = [[AppLovinBannerDelegate alloc] initWithRect:bannerRect key:banner_key];

        NSString* interstitial_key = [NSString stringWithUTF8String:"a34c2956c56e8044"];

        m_interstitialDelegate = [[AppLovinInterstitialDelegate alloc] initWithAdUnitIdentifier:rewarded_key];

        NSString* rewarded_key = [NSString stringWithUTF8String:"5c16834f0cda36ad"];       
                 
        m_rewardedDelegate = [[AppLovinRewardedDelegate alloc] initWithCallback:this AdUnitIdentifier:rewarded_key];       
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::_finalizeService()
    {
        [m_rewardedDelegate release];
        m_rewardedDelegate = nil;
        
        [m_interstitialDelegate release];
        m_interstitialDelegate = nil;
        
        [m_bannerDelegate release];
        m_bannerDelegate = nil;
    }
    /////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::interstitialHasLoaded()
    {
        return [m_interstitialDelegate hasLoaded];
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showInterstitial()
    {
        return [m_interstitialDelegate show];
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::rewardedHasLoaded()
    {
        return [m_rewardedDelegate hasLoaded];
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinService::showRewarded()
    {
        return [m_rewardedDelegate show];
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleAppLovinService::receivedReward( uint64_t amount )
    {
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
}
