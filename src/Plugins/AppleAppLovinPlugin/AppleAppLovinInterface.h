#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"


namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleAppLovinRewardCallbackInterface
        : public Mixin
    {
    public:
        virtual void onAppLovinRewardReceivedReward( uint64_t _amount ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class AppleAppLovinServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleAppLovinService" )

    public:
        virtual bool hasLoadedinterstitial() const override;
        virtual bool showInterstitial() override;

        virtual bool hasLoadedrewarded() const override;
        virtual bool showRewarded() override;

    public:
        virtual void showBanner() override;
        virtual void hideBanner() override;

    public:
        virtual void showMediationDebugger() override;
    };
}
