#pragma once

#include "AppleAppLovinInterface.h"

#include "Kernel/ServiceBase.h"

#import "AppleAppLovinInterstitialDelegate.h"
#import "AppleAppLovinRewardedDelegate.h"
#import "AppleAppLovinBannerDelegate.h"

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
#import "AppleAppLovinAmazonService.h"
#endif

namespace Mengine
{
    class AppleAppLovinService
        : public ServiceBase<AppleAppLovinServiceInterface>
    {
    public:
        AppleAppLovinService();
        ~AppleAppLovinService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        bool initBanner( const ConstString & _adUnitId, const ConstString & _placement, const AppleAppLovinBannerProviderInterfacePtr & _provider ) override;
                
    public:
        bool showBanner( const ConstString & _adUnitId ) override;
        bool hideBanner( const ConstString & _adUnitId ) override;
        
    public:
        bool initInterstitial( const ConstString & _adUnitId, const AppleAppLovinInterstitialProviderInterfacePtr & _provider ) override;
        
    public:
        bool canYouShowInterstitial( const ConstString & _adUnitId, const ConstString & _placement ) const override;
        bool showInterstitial( const ConstString & _adUnitId, const ConstString & _placement ) override;

    public:
        bool initRewarded( const ConstString & _adUnitId, const AppleAppLovinRewardedProviderInterfacePtr & _provider ) override;
        
    public:
        bool canOfferRewarded( const ConstString & _adUnitId, const ConstString & _placement ) const override;
        bool canYouShowRewarded( const ConstString & _adUnitId, const ConstString & _placement ) const override;
        bool showRewarded( const ConstString & _adUnitId, const ConstString & _placement ) override;

    public:
        void showMediationDebugger() override;

    protected:
        AppleAppLovinAnalyticsService * m_analyticsService;
        
        NSMutableDictionary<NSString *, AppleAppLovinBannerDelegate *> * m_banners;
        NSMutableDictionary<NSString *, AppleAppLovinInterstitialDelegate *> * m_interstitials;
        NSMutableDictionary<NSString *, AppleAppLovinRewardedDelegate *> * m_rewardeds;
        
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
        AppleAppLovinAmazonService * m_amazonService;
#endif
    };
}
