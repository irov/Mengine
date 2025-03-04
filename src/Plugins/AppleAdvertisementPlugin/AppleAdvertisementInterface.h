#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Params.h"
#include "Kernel/Viewport.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleAdvertisementBannerProviderInterface
        : public Mixin
    {
    public:
        virtual void onAppleAdvertisementBannerRevenuePaid( const Params & _params ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleAdvertisementBannerProviderInterface> AppleAdvertisementBannerProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleAdvertisementInterstitialProviderInterface
        : public Mixin
    {
    public:
        virtual void onAppleAdvertisementInterstitialShowSuccessful( const Params & _params ) = 0;
        virtual void onAppleAdvertisementInterstitialShowFailed( const Params & _params ) = 0;
        virtual void onAppleAdvertisementInterstitialRevenuePaid( const Params & _params ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleAdvertisementInterstitialProviderInterface> AppleAdvertisementInterstitialProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleAdvertisementRewardedProviderInterface
        : public Mixin
    {
    public:
        virtual void onAppleAdvertisementRewardedShowSuccessful( const Params & _params ) = 0;
        virtual void onAppleAdvertisementRewardedShowFailed( const Params & _params ) = 0;
        virtual void onAppleAdvertisementRewardedRevenuePaid( const Params & _params ) = 0;
        virtual void onAppleAdvertisementRewardedUserRewarded( const Params & _params ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleAdvertisementRewardedProviderInterface> AppleAdvertisementRewardedProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleAdvertisementServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleAdvertisementService" )

    public:
        virtual void setBannerProvider( const AppleAdvertisementBannerProviderInterfacePtr & _bannerProvider ) = 0;
        
    public:
        virtual bool showBanner() = 0;
        virtual bool hideBanner() = 0;
        
        virtual bool getBannerHeight( uint32_t * const _height ) const = 0;
        
    public:
        virtual void setInterstitialProvider( const AppleAdvertisementInterstitialProviderInterfacePtr & _interstitialProvider ) = 0;
        
    public:
        virtual bool canYouShowInterstitial( const ConstString & _placement ) const = 0;
        virtual bool showInterstitial( const ConstString & _placement ) = 0;
        
    public:
        virtual void setRewardedProvider( const AppleAdvertisementRewardedProviderInterfacePtr & _rewardedProvider ) = 0;
        
    public:
        virtual bool canOfferRewarded( const ConstString & _placement ) const = 0;
        virtual bool canYouShowRewarded( const ConstString & _placement ) const = 0;
        virtual bool showRewarded( const ConstString & _placement ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_ADVERTISEMENT_SERVICE()\
    ((Mengine::AppleAdvertisementServiceInterface *)SERVICE_GET(Mengine::AppleAdvertisementServiceInterface))
//////////////////////////////////////////////////////////////////////////
