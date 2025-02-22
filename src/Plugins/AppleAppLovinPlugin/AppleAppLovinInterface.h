#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Params.h"
#include "Kernel/Viewport.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleAppLovinBannerProviderInterface
        : public Mixin
    {
    public:
        virtual void onAppleAppLovinBannerRevenuePaid( const Params & _params ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleAppLovinBannerProviderInterface> AppleAppLovinBannerProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleAppLovinInterstitialProviderInterface
        : public Mixin
    {
    public:
        virtual void onAppleAppLovinInterstitialShowSuccessful( const Params & _params ) = 0;
        virtual void onAppleAppLovinInterstitialShowFailed( const Params & _params ) = 0;
        virtual void onAppleAppLovinInterstitialRevenuePaid( const Params & _params ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleAppLovinInterstitialProviderInterface> AppleAppLovinInterstitialProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleAppLovinRewardedProviderInterface
        : public Mixin
    {
    public:
        virtual void onAppleAppLovinRewardedShowSuccessful( const Params & _params ) = 0;
        virtual void onAppleAppLovinRewardedShowFailed( const Params & _params ) = 0;
        virtual void onAppleAppLovinRewardedRevenuePaid( const Params & _params ) = 0;
        virtual void onAppleAppLovinRewardedUserRewarded( const Params & _params ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleAppLovinRewardedProviderInterface> AppleAppLovinRewardedProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleAppLovinConsentFlowProviderInterface
        : public Mixin
    {
    public:
        virtual void onAppleAppLovinConsentFlowShowSuccessful() = 0;
        virtual void onAppleAppLovinConsentFlowShowFailed() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleAppLovinConsentFlowProviderInterface> AppleAppLovinConsentFlowProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleAppLovinServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleAppLovinService" )

    public:
        virtual void setBannerProvider( const AppleAppLovinBannerProviderInterfacePtr & _bannerProvider ) = 0;
        
    public:
        virtual bool showBanner() = 0;
        virtual bool hideBanner() = 0;
        
        virtual bool getBannerHeight( uint32_t * const _height ) const = 0;
        
    public:
        virtual void setInterstitialProvider( const AppleAppLovinInterstitialProviderInterfacePtr & _interstitialProvider ) = 0;
        
    public:
        virtual bool canYouShowInterstitial( const ConstString & _placement ) const = 0;
        virtual bool showInterstitial( const ConstString & _placement ) = 0;
        
    public:
        virtual void setRewardedProvider( const AppleAppLovinRewardedProviderInterfacePtr & _rewardedProvider ) = 0;
        
    public:
        virtual bool canOfferRewarded( const ConstString & _placement ) const = 0;
        virtual bool canYouShowRewarded( const ConstString & _placement ) const = 0;
        virtual bool showRewarded( const ConstString & _placement ) = 0;
        
    public:
        virtual bool hasSupportedCMP() const = 0;
        virtual bool isConsentFlowUserGeographyGDPR() const = 0;
        virtual void loadAndShowCMPFlow( const AppleAppLovinConsentFlowProviderInterfacePtr & _provider ) = 0;
        
    public:
        virtual void showMediationDebugger() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_APPLOVIN_SERVICE()\
    ((Mengine::AppleAppLovinServiceInterface *)SERVICE_GET(Mengine::AppleAppLovinServiceInterface))
//////////////////////////////////////////////////////////////////////////
