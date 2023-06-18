#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class AppleAppLovinServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleAppLovinService" )

    public:
        virtual void initBanner( const ConstString & _bannerAdUnit ) = 0;
        
    public:
        virtual void initInterstitial( const ConstString & _interstitialAdUnit ) = 0;
        
    public:
        virtual bool canYouShowInterstitial() const = 0;
        virtual bool showInterstitial() = 0;
        
    public:
        virtual void initRewarded( const ConstString & _rewardedAdUnit ) = 0;

        virtual bool canOfferRewarded() const = 0;
        virtual bool canYouShowRewarded() const = 0;
        virtual bool showRewarded() = 0;

    public:
        virtual void showBanner() = 0;
        virtual void hideBanner() = 0;

    public:
        virtual void showMediationDebugger() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_APPLOVIN_SERVICE()\
    ((Mengine::AppleAppLovinServiceInterface *)SERVICE_GET(Mengine::AppleAppLovinServiceInterface))
//////////////////////////////////////////////////////////////////////////
