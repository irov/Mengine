#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/Map.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Map<String, String> MARSDKResultParams;
    //////////////////////////////////////////////////////////////////////////
    class AppleMARSDKProviderInterface
        : public ServantInterface
    {
    public:
        virtual void onPlatformInit( const MARSDKResultParams & _params ) = 0;
        virtual void onRealName( const MARSDKResultParams & _params ) = 0;
        virtual void onEventWithCode( int32_t _code, const Char * _msg ) = 0;
        virtual void onEventCustom( const Char * _eventName, const MARSDKResultParams & _params ) = 0;
        
    public:
        virtual void onUserLogin( const MARSDKResultParams & _params ) = 0;
        virtual void onUserLogout( const MARSDKResultParams & _params ) = 0;
        virtual void onPayPaid( const MARSDKResultParams & _params ) = 0;
        
    public:
        virtual void onPropComplete( const ConstString & _orderId ) = 0;
        virtual void onPropError( const ConstString & _orderId ) = 0;
        
    public:
        virtual void onPurchasedNonConsumable( const VectorConstString & _purchased ) = 0;
        
    public:
        virtual void onAdRewardedDidFailed() = 0;
        virtual void onAdRewardedDidLoaded() = 0;
        virtual void onAdRewardedDidShow() = 0;
        virtual void onAdRewardedDidClicked() = 0;
        virtual void onAdRewardedDidClosed() = 0;
        virtual void onAdRewardedDidSkipped() = 0;
        virtual void onAdRewardedDidFinished( const Char * _itemName, uint32_t _itemNum ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleMARSDKProviderInterface> AppleMARSDKProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleMARSDKServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleMARSDKService" )

    public:
        virtual void setProvider( const AppleMARSDKProviderInterfacePtr & _provider ) = 0;
        virtual const AppleMARSDKProviderInterfacePtr & getProvider() const = 0;
        
    public:
        virtual bool login() = 0;
        virtual bool logout() = 0;
        virtual bool switchAccount() = 0;
        
    public:
        virtual void requestNonConsumablePurchased() = 0;
        
    public:
        virtual void submitExtendedData( const Char * _data ) = 0;
        virtual void submitPaymentData( const Char * _data ) = 0;
        
        virtual void propComplete( const ConstString & _productId ) = 0;
        
    public:
        virtual void showRewardVideoAd( const ConstString & _itemName, uint32_t _itemNum ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_MARSDK_SERVICE()\
    ((Mengine::AppleMARSDKServiceInterface *)SERVICE_GET(Mengine::AppleMARSDKServiceInterface))
//////////////////////////////////////////////////////////////////////////
