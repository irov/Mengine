#include "AppLovinService.h"

#include "Kernel/Logger.h"

#include "Config/Algorithm.h"

 #import "AppLovinRewardedDelegate.h"

//////////////////////////////////////////////////////////////////////////
//SERVICE_FACTORY( AppLovinService, Mengine::AppLovinService );
/////////////////////////////////////////////////////////////////////////////
@interface AppLovinCallbacks : NSObject<AppLovinRewardCallbackProtocol>
//////////////////////////////////////////////////////////////////////////
- (void) receivedReward:(int) amount;
//////////////////////////////////////////////////////////////////////////
@property Mengine::AppLovinService * m_service;
//////////////////////////////////////////////////////////////////////////
- (instancetype _Nonnull)initWithCallback:(Mengine::AppLovinService * _Nonnull) service;
//////////////////////////////////////////////////////////////////////////
@end
//////////////////////////////////////////////////////////////////////////
@implementation AppLovinCallbacks
//////////////////////////////////////////////////////////////////////////
#pragma mark - AppLovinCallbacks
//////////////////////////////////////////////////////////////////////////
@synthesize m_service;
//////////////////////////////////////////////////////////////////////////
- (instancetype)initWithCallback:(Mengine::AppLovinService * _Nonnull) service {
    m_service = service;
}
//////////////////////////////////////////////////////////////////////////
- (void) receivedReward:(int) amount{
    m_service->receivedReward(amount);
}
//////////////////////////////////////////////////////////////////////////
@end
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppLovinService::AppLovinService()
        : m_interstitialDelegate(nil),
          m_rewardedDelegate(nil),
          m_bannerDelegate(nil){
    }
    //////////////////////////////////////////////////////////////////////////
    AppLovinService::~AppLovinService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppLovinService::_initializeService()
    {
        NSString* interstitial_key = [NSString stringWithUTF8String:"a34c2956c56e8044"];
        NSString* rewarded_key = [NSString stringWithUTF8String:"5c16834f0cda36ad"];
        NSString* banner_key = [NSString stringWithUTF8String:"6c5e2fd2fa0e48e6"];
        
        id<AppLovinRewardCallbackProtocol> * callbacks = (id<AppLovinRewardCallbackProtocol> *)[[AppLovinCallbacks alloc] initWithCallback:this];
                
        
        m_rewardedDelegate = [[AppLovinRewardedDelegate alloc] initWithCallback:callbacks AdUnitIdentifier:rewarded_key];
        m_interstitialDelegate = [[AppLovinInterstitialDelegate alloc] initWithAdUnitIdentifier:rewarded_key];
        
        
        // Banner height on iPhone and iPad is 50 and 90, respectively
        CGFloat height = (UIDevice.currentDevice.userInterfaceIdiom == UIUserInterfaceIdiomPad) ? 90 : 50;
        // Stretch to the width of the screen for banners to be fully functional
        CGFloat width = CGRectGetWidth(UIScreen.mainScreen.bounds);
        CGRect bannerRect = CGRectMake(0, 0, width, height);
        
        m_bannerDelegate = [[AppLovinBannerDelegate alloc] initWithRect:bannerRect key:banner_key];
        
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppLovinService::_finalizeService()
    {
        [m_rewardedDelegate release];
        m_rewardedDelegate = nil;
        
        [m_interstitialDelegate release];
        m_interstitialDelegate = nil;
        
        [m_bannerDelegate release];
        m_bannerDelegate = nil;
    }
    //////////////////////////////////////////////////////////////////////
//    void AppLovinService::setProvider( const AppleMARSDKProviderInterfacePtr & _provider )
//    {
//        m_provider = _provider;
//    }
    //////////////////////////////////////////////////////////////////////
//    const AppleMARSDKProviderInterfacePtr & AppLovinService::getProvider() const
//    {
//        return m_provider;
//    }
    /////////////////////////////////////////////////////////////////////////
    bool AppLovinService::interstitialHasLoaded(){
        return [m_interstitialDelegate hasLoaded];
    }
//    //////////////////////////////////////////////////////////////////////
    bool AppLovinService::showInterstitial(){
        return [m_interstitialDelegate show];
    }
//    //////////////////////////////////////////////////////////////////////
    bool AppLovinService::rewardedHasLoaded(){
        return [m_rewardedDelegate hasLoaded];
    }
//    //////////////////////////////////////////////////////////////////////
    bool AppLovinService::showRewarded(){
        return [m_rewardedDelegate show];
    }
//    //////////////////////////////////////////////////////////////////////
    void AppLovinService::receivedReward(int amount){
    }
//    //////////////////////////////////////////////////////////////////////
    void AppLovinService::visibleBanner(bool show){
        if(show){
            [m_bannerDelegate show];
        }else{
            [m_bannerDelegate hide];
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
