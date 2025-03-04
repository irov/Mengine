#pragma once

#include "AppleAdvertisementInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleAdvertisementService
        : public ServiceBase<AppleAdvertisementServiceInterface>
    {
    public:
        AppleAdvertisementService();
        ~AppleAdvertisementService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        void setBannerProvider( const AppleAdvertisementBannerProviderInterfacePtr & _bannerProvider ) override;
                
    public:
        bool showBanner() override;
        bool hideBanner() override;
        
    public:
        bool getBannerHeight( uint32_t * const _height ) const override;
        
    public:
        void setInterstitialProvider( const AppleAdvertisementInterstitialProviderInterfacePtr & _interstitialProvider ) override;
        
    public:
        bool canYouShowInterstitial( const ConstString & _placement ) const override;
        bool showInterstitial( const ConstString & _placement ) override;

    public:
        void setRewardedProvider( const AppleAdvertisementRewardedProviderInterfacePtr & _rewardedProvider ) override;
        
    public:
        bool canOfferRewarded( const ConstString & _placement ) const override;
        bool canYouShowRewarded( const ConstString & _placement ) const override;
        bool showRewarded( const ConstString & _placement ) override;
    };
}
