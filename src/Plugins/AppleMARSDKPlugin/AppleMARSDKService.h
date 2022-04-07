#pragma once

#include "AppleMARSDKInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"

#include "AppleMARSDKDelegate.h"

namespace Mengine
{

enum DataType{
    TYPE_SELECT_SERVER = 1,
    TYPE_CREATE_ROLE,
    TYPE_ENTER_GAME,
    TYPE_LEVEL_UP,
    TYPE_EXIT_GAME
};

struct ExtraData{
    
    DataType dataType;
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

struct ProductInfo{
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

	class AppleMARSDKService
		: public ServiceBase<AppleMARSDKServiceInterface>
	{
	public:
        AppleMARSDKService();
		~AppleMARSDKService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        bool login();
        bool logout();
        bool switchAccount();
        
        void submitExtendedData(const Mengine::ExtraData& extradata);
        void submitPayementData(const Mengine::ProductInfo& product);

	protected:
        AppleMARSDKDelegate * m_appleMARSDKNative;
    };
}
