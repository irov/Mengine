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
        void initBanner( const ConstString & _bannerAdUnit ) override;
        
    public:
        void initInterstitial( const ConstString & _interstitialAdUnit ) override;
        
    public:
        bool canYouShowInterstitial() const override;
        bool showInterstitial() override;

    public:
        void initRewarded( const ConstString & _rewardedAdUnit ) override;
        
    public:
        bool canOfferRewarded() const override;
        bool canYouShowRewarded() const override;
        bool showRewarded() override;

    public:
        void showBanner() override;
        void hideBanner() override;

    public:
        void showMediationDebugger() override;

    protected:
        AppleAppLovinAnalyticsService * m_analyticsService;
        
        AppleAppLovinBannerDelegate * m_banner;
        AppleAppLovinInterstitialDelegate * m_interstitial;
        AppleAppLovinRewardedDelegate * m_rewarded;
        
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
        AppleAppLovinAmazonService * m_amazonService;
#endif
    };
}
