#include "AppleMARSDKService.h"

#include "Environment/iOS/iOSDetail.h"
#include "Environment/Apple/AppleString.h"

#include "Kernel/Logger.h"

#include "Config/Algorithm.h"

#import "MARSDKCore/MARSDKCore.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleMARSDKService, Mengine::AppleMARSDKService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleMARSDKService::AppleMARSDKService()
        : m_delegate( nil )
        , m_adRewardedDelegate( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleMARSDKService::~AppleMARSDKService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKService::_initializeService()
    {
        m_delegate = [[AppleMARSDKDelegate alloc] initWithService:this];
        
        [[MARSDK sharedInstance] setDelegate:m_delegate];
        
        m_adRewardedDelegate = [[AppleMARSDKAdRewardedDelegate alloc] initWithService:this];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::_finalizeService()
    {
        [[MARSDK sharedInstance] setDelegate:nil];
        
        if( m_delegate != nil )
        {
            [m_delegate release];
            m_delegate = nil;
        }
        
        if( m_adRewardedDelegate != nil )
        {
            [m_adRewardedDelegate release];
            m_adRewardedDelegate = nil;
        }
    }
    //////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::setProvider( const AppleMARSDKProviderInterfacePtr & _provider )
    {
        m_provider = _provider;
    }
    //////////////////////////////////////////////////////////////////////
    const AppleMARSDKProviderInterfacePtr & AppleMARSDKService::getProvider() const
    {
        return m_provider;
    }
    //////////////////////////////////////////////////////////////////////
    bool AppleMARSDKService::login()
    {
        LOGGER_INFO( "marsdk", "login" );
        
        [[MARSDK sharedInstance] login];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKService::logout()
    {
        LOGGER_INFO( "marsdk", "logout" );
        
        [[MARSDK sharedInstance] logout];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKService::switchAccount()
    {
        LOGGER_INFO( "marsdk", "switchAccount" );
        
        [[MARSDK sharedInstance] switchAccount];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::submitExtendedData( const MARSDKExtraData & _data )
    {
        MARUserExtraData* extraData = [[MARUserExtraData alloc] init];
        
        extraData.dataType = _data.dataType;
        extraData.opType = [NSString stringWithUTF8String: _data.opType.c_str()];
        extraData.roleID = [NSString stringWithUTF8String: _data.roleID.c_str()];
        extraData.roleName = [NSString stringWithUTF8String: _data.roleName.c_str()];
        extraData.roleLevel = [NSString stringWithUTF8String: _data.roleLevel.c_str()];
        extraData.serverID = _data.serverId;
        extraData.serverName = [NSString stringWithUTF8String: _data.serverName.c_str()];
        extraData.moneyNum = _data.moneyNum;
        extraData.roleCreateTime = _data.roleCreateTime;
        extraData.roleLevelUpTime = _data.roleLevelUpTime;
        
        extraData.vip = [NSString stringWithUTF8String: _data.vip.c_str()];
        
        extraData.roleGender = _data.roleGender;
        extraData.professionID = [NSString stringWithUTF8String: _data.professionID.c_str()];
        extraData.professionName = [NSString stringWithUTF8String: _data.professionName.c_str()];
        extraData.power = [NSString stringWithUTF8String: _data.power.c_str()];
        extraData.partyID = [NSString stringWithUTF8String: _data.partyID.c_str()];
        extraData.partyName = [NSString stringWithUTF8String: _data.partyName.c_str()];
        extraData.partyMasterID = [NSString stringWithUTF8String: _data.partyMasterID.c_str()];
        extraData.partyMasterName = [NSString stringWithUTF8String: _data.partyMasterName.c_str()];
        
        LOGGER_INFO( "marsdk", "submit extended data: %s"
            , [[extraData toJsonString] UTF8String]
        );
        
        [[MARSDK sharedInstance] submitExtraData:extraData];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::submitPaymentData( const MARSDKProductInfo & _info )
    {
        MARProductInfo* productInfo = [[MARProductInfo alloc] init];
        
        productInfo.orderID = [NSString stringWithUTF8String:_info.orderID.c_str()];
        productInfo.productId = [NSString stringWithUTF8String:_info.productId.c_str()];
        productInfo.productName = [NSString stringWithUTF8String:_info.productName.c_str()];
        productInfo.productDesc = [NSString stringWithUTF8String:_info.productDesc.c_str()];
        
        productInfo.payType = _info.payType;
        
        productInfo.price = [NSNumber numberWithInt:_info.price];
        productInfo.buyNum = _info.buyNum;
        productInfo.coinNum = _info.coinNum;
        
        productInfo.roleId = [NSString stringWithUTF8String: _info.roleId.c_str()];
        productInfo.roleName = [NSString stringWithUTF8String: _info.roleName.c_str()];
        productInfo.roleLevel = [NSString stringWithUTF8String: _info.roleLevel.c_str()];
        productInfo.vip = [NSString stringWithUTF8String: _info.vip.c_str()];
        productInfo.serverId = [NSString stringWithUTF8String: _info.serverId.c_str()];
        productInfo.serverName = [NSString stringWithUTF8String: _info.serverName.c_str()];
        productInfo.notifyUrl = [NSString stringWithUTF8String: _info.notifyUrl.c_str()];

        LOGGER_INFO( "marsdk", "submit payment data: %s"
            , [[productInfo toJsonString] UTF8String]
        );
        
        [[MARSDK sharedInstance] pay:productInfo];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::showRewardVideoAd( const ConstString & _itemName, uint32_t _itemNum )
    {
        LOGGER_INFO( "marsdk", "show reward video ad: item '%s' num [%u]"
            , _itemName.c_str()
            , _itemNum
        );
        
        UIViewController * rootViewController = Helper::iOSGetRootViewController();
        
        [[MARAd sharedInstance] showRewardVideoAd:rootViewController
                                         itemName:[[NSString alloc] initWithUTF8String:_itemName.c_str()]
                                          itemNum:_itemNum delegate:m_adRewardedDelegate];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::onUserLogin( const MARSDKResultParams & _params )
    {
        LOGGER_INFO( "marsdk", "event user login" );
        
        if( m_provider == nullptr )
        {
            return;
        }
        
        m_provider->onUserLogin( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::onUserLogout( const MARSDKResultParams & _params )
    {
        LOGGER_INFO( "marsdk", "event user logout" );
        
        if( m_provider == nullptr )
        {
            return;
        }
        
        m_provider->onUserLogout( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::onPayPaid( const MARSDKResultParams & _params )
    {
        LOGGER_INFO( "marsdk", "event pay paid" );
        
        if( m_provider == nullptr )
        {
            return;
        }
        
        m_provider->onPayPaid( _params );
    }
    //////////////////////////////////////////////////////////////////////////
}
