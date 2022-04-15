#pragma once

#include "AppleAppLovinInterface.h"

#include "AppLovinInterstitialDelegate.h"
#include "AppLovinRewardedDelegate.h"
#include "AppLovinBannerDelegate.h"

#include "Kernel/ServiceBase.h"

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
        bool hasLoadedinterstitial() const;
        bool showInterstitial();

        bool hasLoadedrewarded() const;
        bool showRewarded();

    public:
        void showBanner();
        void hideBanner();

    public:
        void showMediationDebugger();

    protected:
        void onAppLovinRewardReceivedReward( uint64_t _amount );

    protected:
        AppleAppLovinInterstitialDelegate * m_interstitial;
        AppleAppLovinRewardedDelegate * m_rewarded;
        AppleAppLovinBannerDelegate * m_banner;
    };
}
