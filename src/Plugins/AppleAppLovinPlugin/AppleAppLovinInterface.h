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
    typedef IntrusivePtr<AppleAppLovinRewardCallbackInterface> AppleAppLovinRewardCallbackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleAppLovinServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleAppLovinService" )

    public:
        virtual bool hasLoadedinterstitial() const = 0;
        virtual bool showInterstitial() = 0;

        virtual bool hasLoadedrewarded() const = 0;
        virtual bool showRewarded() = 0;

    public:
        virtual void showBanner() = 0;
        virtual void hideBanner() = 0;

    public:
        virtual void showMediationDebugger() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
