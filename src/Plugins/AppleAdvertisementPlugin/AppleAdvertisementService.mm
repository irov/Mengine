#include "AppleAdvertisementService.h"

#import "Environment/Apple/AppleString.h"
#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSNetwork.h"

#import "AppleAdvertisementApplicationDelegate.h"

#include "Kernel/Assertion.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleAdvertisementService, Mengine::AppleAdvertisementService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAdvertisementService::AppleAdvertisementService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAdvertisementService::~AppleAdvertisementService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAdvertisementService::_initializeService()
    {
        //Empty
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleAdvertisementService::_finalizeService()
    {
        //Empty
    }
    /////////////////////////////////////////////////////////////////////////
    void AppleAdvertisementService::setBannerProvider( const AppleAdvertisementBannerProviderInterfacePtr & _bannerProvider )
    {
        [[AppleAppLovinApplicationDelegate sharedInstance] setBannerProvider:_bannerProvider];
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAdvertisementService::showBanner()
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
    bool AppleAdvertisementService::hideBanner()
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
    bool AppleAdvertisementService::getBannerHeight( uint32_t * const _height ) const
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
    void AppleAdvertisementService::setInterstitialProvider( const AppleAdvertisementInterstitialProviderInterfacePtr & _interstitialProvider )
    {
        [[AppleAppLovinApplicationDelegate sharedInstance] setInterstitialProvider:_interstitialProvider];
    }
    /////////////////////////////////////////////////////////////////////////
    bool AppleAdvertisementService::canYouShowInterstitial( const ConstString & _placement ) const
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
    bool AppleAdvertisementService::showInterstitial( const ConstString & _placement )
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
    bool AppleAdvertisementService::canOfferRewarded( const ConstString & _placement ) const
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
    bool AppleAdvertisementService::canYouShowRewarded( const ConstString & _placement ) const
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
    void AppleAdvertisementService::setRewardedProvider( const AppleAdvertisementRewardedProviderInterfacePtr & _rewardedProvider )
    {
        [[AppleAppLovinApplicationDelegate sharedInstance] setRewardedProvider:_rewardedProvider];
    }
    ////////////////////////////////////////////////////////////////////////
    bool AppleAdvertisementService::showRewarded( const ConstString & _placement )
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
}
