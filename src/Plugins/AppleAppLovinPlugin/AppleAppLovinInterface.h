#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleAppLovinBannerProviderInterface
        : public Interface
    {
    public:
        virtual void onAppleAppLovinBannerDidStartAdRequestForAdUnitIdentifier() = 0;
        virtual void onAppleAppLovinBannerDidLoadAd() = 0;
        virtual void onAppleAppLovinBannerDidFailToLoadAdForAdUnitIdentifier() = 0;
        virtual void onAppleAppLovinBannerDidClickAd() = 0;
        virtual void onAppleAppLovinBannerDidFailToDisplayAd() = 0;
        virtual void onAppleAppLovinBannerDidExpandAd() = 0;
        virtual void onAppleAppLovinBannerDidCollapseAd() = 0;
        virtual void onAppleAppLovinBannerDidPayRevenueForAd() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleAppLovinBannerProviderInterface> AppleAppLovinBannerProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleAppLovinInterstitailProviderInterface
        : public Interface
    {
    public:
        virtual void onAppleAppLovinInterstitialDidStartAdRequestForAdUnitIdentifier() = 0;
        virtual void onAppleAppLovinInterstitialDidLoadAd() = 0;
        virtual void onAppleAppLovinInterstitialDidFailToLoadAdForAdUnitIdentifier() = 0;
        virtual void onAppleAppLovinInterstitialDidDisplayAd() = 0;
        virtual void onAppleAppLovinInterstitialDidClickAd() = 0;
        virtual void onAppleAppLovinInterstitialDidHideAd() = 0;
        virtual void onAppleAppLovinInterstitialDidFailToDisplayAd() = 0;
        virtual void onAppleAppLovinInterstitialDidPayRevenueForAd() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleAppLovinInterstitailProviderInterface> AppleAppLovinInterstitialProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleAppLovinRewardedProviderInterface
        : public Interface
    {
    public:
        virtual void onAppleAppLovinRewardedDidStartAdRequestForAdUnitIdentifier() = 0;
        virtual void onAppleAppLovinRewardedDidLoadAd() = 0;
        virtual void onAppleAppLovinRewardedDidFailToLoadAdForAdUnitIdentifier() = 0;
        virtual void onAppleAppLovinRewardedDidDisplayAd() = 0;
        virtual void onAppleAppLovinRewardedDidClickAd() = 0;
        virtual void onAppleAppLovinRewardedDidHideAd() = 0;
        virtual void onAppleAppLovinRewardedDidFailToDisplayAd() = 0;
        virtual void onAppleAppLovinRewardedDidStartRewardedVideoForAd() = 0;
        virtual void onAppleAppLovinRewardedDidCompleteRewardedVideoForAd() = 0;
        virtual void onAppleAppLovinRewardedDidRewardUserForAd() = 0;
        virtual void onAppleAppLovinRewardedDidPayRevenueForAd() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleAppLovinRewardedProviderInterface> AppleAppLovinRewardedProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleAppLovinConsentFlowProviderInterface
        : public Interface
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
        virtual bool initBanner( const ConstString & _adUnitId, const ConstString & _placement, const AppleAppLovinBannerProviderInterfacePtr & _provider ) = 0;
        
    public:
        virtual bool showBanner( const ConstString & _adUnitId ) = 0;
        virtual bool hideBanner( const ConstString & _adUnitId ) = 0;
        
    public:
        virtual bool initInterstitial( const ConstString & _adUnitId, const AppleAppLovinInterstitialProviderInterfacePtr & _provider ) = 0;
        
    public:
        virtual bool canYouShowInterstitial( const ConstString & _adUnitId, const ConstString & _placement ) const = 0;
        virtual bool showInterstitial( const ConstString & _adUnitId, const ConstString & _placement ) = 0;
        
    public:
        virtual bool initRewarded( const ConstString & _adUnitId, const AppleAppLovinRewardedProviderInterfacePtr & _provider ) = 0;

        virtual bool canOfferRewarded( const ConstString & _adUnitId, const ConstString & _placement ) const = 0;
        virtual bool canYouShowRewarded( const ConstString & _adUnitId, const ConstString & _placement ) const = 0;
        virtual bool showRewarded( const ConstString & _adUnitId, const ConstString & _placement ) = 0;
        
    public:
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
