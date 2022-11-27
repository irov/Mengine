#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Map.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EMARSDKDataType
    {
        TYPE_SELECT_SERVER = 1,
        TYPE_CREATE_ROLE,
        TYPE_ENTER_GAME,
        TYPE_LEVEL_UP,
        TYPE_EXIT_GAME
    };
    //////////////////////////////////////////////////////////////////////////
    struct MARSDKExtraData
    {
        EMARSDKDataType dataType;
        ConstString opType;
        ConstString roleID;
        ConstString roleName;
        ConstString roleLevel;
        int32_t serverID;
        ConstString serverName;
        int32_t moneyNum;
        int64_t roleCreateTime;//time(NULL)
        int64_t roleLevelUpTime;//time(NULL)

        ConstString vip;
        int32_t roleGender;
        ConstString professionID;
        ConstString professionName;
        ConstString power;
        ConstString partyID;
        ConstString partyName;
        ConstString partyMasterID;
        ConstString partyMasterName;
        
        int32_t serverId;
    };
    //////////////////////////////////////////////////////////////////////////
    struct MARSDKProductInfo
    {
        ConstString orderID;
        ConstString productId;
        String productName;
        String productDesc;
        
        int32_t payType;

        double price;
        int32_t buyNum;
        int32_t coinNum;

        ConstString roleId;
        ConstString roleName;
        ConstString roleLevel;
        ConstString vip;
        ConstString serverId;
        ConstString serverName;
        ConstString notifyUrl;
    };
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
        
        virtual void submitExtendedData( const MARSDKExtraData & _data ) = 0;
        virtual void submitPaymentData( const MARSDKProductInfo & _info ) = 0;
        
        virtual void showRewardVideoAd( const ConstString & _itemName, uint32_t _itemNum ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_MARSDK_SERVICE()\
    ((Mengine::AppleMARSDKServiceInterface *)SERVICE_GET(Mengine::AppleMARSDKServiceInterface))
//////////////////////////////////////////////////////////////////////////
