#pragma once

#include "AppleAppLovinInterface.h"

#include "Kernel/ServiceBase.h"

#import "AppleAppLovinInterstitialDelegate.h"
#import "AppleAppLovinRewardedDelegate.h"
#import "AppleAppLovinBannerDelegate.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
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
        void setProvider( const AppleAppLovinProviderInterfacePtr & _provider ) override;
                
    public:
        bool showBanner() override;
        bool hideBanner() override;
        
    public:
        bool getBannerHeight( uint32_t * const _height ) const override;
        bool getBannerViewport( Viewport * const _viewport ) const override;
        
    public:
        bool canYouShowInterstitial( const ConstString & _placement ) const override;
        bool showInterstitial( const ConstString & _placement, const AppleAppLovinInterstitialProviderInterfacePtr & _interstitialProvider ) override;

    public:
        bool canOfferRewarded( const ConstString & _placement ) const override;
        bool canYouShowRewarded( const ConstString & _placement ) const override;
        bool showRewarded( const ConstString & _placement, const AppleAppLovinRewardedProviderInterfacePtr & _rewardedProvider ) override;
        
    public:
        bool hasSupportedCMP() const override;
        bool isConsentFlowUserGeographyGDPR() const override;
        void loadAndShowCMPFlow( const AppleAppLovinConsentFlowProviderInterfacePtr & _rewardedProvider ) override;

    public:
        void showMediationDebugger() override;
    };
}
