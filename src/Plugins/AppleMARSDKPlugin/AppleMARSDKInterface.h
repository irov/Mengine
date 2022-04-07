#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

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
        int serverID;
        ConstString serverName;
        int moneyNum;
        long roleCreateTime;//time(NULL)
        long roleLevelUpTime;//time(NULL)

        ConstString vip;
        int roleGender;
        ConstString professionID;
        ConstString professionName;
        ConstString power;
        ConstString partyID;
        ConstString partyName;
        ConstString partyMasterID;
        ConstString partyMasterName;
        
        int serverId;
    };
    //////////////////////////////////////////////////////////////////////////
    struct MARSDKProductInfo
    {
        ConstString orderID;
        ConstString productId;
        ConstString productName;
        ConstString productDesc;

        double price;
        int buyNum;
        int coinNum;

        ConstString roleId;
        ConstString roleName;
        ConstString roleLevel;
        ConstString vip;
        ConstString serverId;
        ConstString serverName;
        ConstString notifyUrl;
    };
    //////////////////////////////////////////////////////////////////////////
    class AppleMARSDKProviderInterface
        : public ServantInterface
    {
    public:
        virtual void onUserLogin() = 0;
        virtual void onUserLogout() = 0;
        virtual void onPayPaid() = 0;
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
        virtual void submitPayementData( const MARSDKProductInfo & _info ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_MARSDK_SERVICE()\
    ((Mengine::AppleMARSDKServiceInterface *)SERVICE_GET(Mengine::AppleMARSDKServiceInterface))
//////////////////////////////////////////////////////////////////////////
