#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class AppleAppLovinServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleAppLovinService" )

    public:
        virtual void initBanner() = 0;
        virtual void initInterstitial() = 0;
        virtual void initRewarded() = 0;
        
    public:
        virtual bool hasLoadedInterstitial() const = 0;
        virtual bool showInterstitial() = 0;

        virtual bool hasLoadedRewarded() const = 0;
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
    ((AppleAppLovinServiceInterface *)SERVICE_GET(Mengine::AppleAppLovinServiceInterface))
//////////////////////////////////////////////////////////////////////////
