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
        , public AppleAppLovinRewardCallbackInterface
    {
    public:
        AppleAppLovinService();
        ~AppleAppLovinService();

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        void initBanner() override;
        void initInterstitial() override;
        void initRewarded() override;

    public:
        bool hasLoadedInterstitial() const override;
        bool showInterstitial() override;

        bool hasLoadedRewarded() const override;
        bool showRewarded() override;

    public:
        void showBanner() override;
        void hideBanner() override;

    public:
        void showMediationDebugger() override;

    protected:
        void onAppLovinRewardReceivedReward( uint64_t _amount ) override;

    protected:
        AppleAppLovinBannerDelegate * m_banner;
        AppleAppLovinInterstitialDelegate * m_interstitial;
        AppleAppLovinRewardedDelegate * m_rewarded;
        
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
        AppleAppLovinAmazonService * m_amazonService;
#endif
    };
}
