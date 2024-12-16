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
        bool initBanner( const ConstString & _adUnitId, const ConstString & _placement, const AppleAppLovinBannerProviderInterfacePtr & _provider ) override;
                
    public:
        bool showBanner( const ConstString & _adUnitId ) override;
        bool hideBanner( const ConstString & _adUnitId ) override;
        
    public:
        bool getBannerViewport( const ConstString & _adUnitId, Viewport * const _viewport ) const override;
        
    public:
        bool initInterstitial( const ConstString & _adUnitId, const AppleAppLovinInterstitialProviderInterfacePtr & _provider ) override;
        
    public:
        bool canYouShowInterstitial( const ConstString & _adUnitId, const ConstString & _placement ) const override;
        bool showInterstitial( const ConstString & _adUnitId, const ConstString & _placement ) override;

    public:
        bool initRewarded( const ConstString & _adUnitId, const AppleAppLovinRewardedProviderInterfacePtr & _provider ) override;
        
    public:
        bool canOfferRewarded( const ConstString & _adUnitId, const ConstString & _placement ) const override;
        bool canYouShowRewarded( const ConstString & _adUnitId, const ConstString & _placement ) const override;
        bool showRewarded( const ConstString & _adUnitId, const ConstString & _placement ) override;
        
    public:
        bool hasSupportedCMP() const override;
        bool isConsentFlowUserGeographyGDPR() const override;
        void loadAndShowCMPFlow( const AppleAppLovinConsentFlowProviderInterfacePtr & _provider ) override;

    public:
        void showMediationDebugger() override;
        
    protected:        
        NSMutableDictionary<NSString *, AppleAppLovinBannerDelegate *> * m_banners;
        NSMutableDictionary<NSString *, AppleAppLovinInterstitialDelegate *> * m_interstitials;
        NSMutableDictionary<NSString *, AppleAppLovinRewardedDelegate *> * m_rewardeds;
        
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
        AppleAppLovinAmazonService * m_amazonService;
#endif
    };
}
