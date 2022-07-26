#include "AppleMARSDKService.h"

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
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::_finalizeService()
    {
        [m_delegate release];
        m_delegate = nil;
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
        
        [m_delegate login];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKService::logout()
    {
        LOGGER_INFO( "marsdk", "logout" );
        
        [m_delegate logout];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKService::switchAccount()
    {
        LOGGER_INFO( "marsdk", "switchAccount" );
        
        [m_delegate switchAccount];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::submitExtendedData( const MARSDKExtraData & _data )
    {
        LOGGER_INFO( "marsdk", "submit extended data" );
        
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
        
        [m_delegate submitExtendedData:extraData];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::submitPaymentData( const MARSDKProductInfo & _info )
    {
        LOGGER_INFO( "marsdk", "submit payment data" );
        
        MARProductInfo* productInfo = [[MARProductInfo alloc] init];
        
        productInfo.orderID = [NSString stringWithUTF8String:_info.orderID.c_str()];
        productInfo.productId = [NSString stringWithUTF8String:_info.productId.c_str()];
        productInfo.productName = [NSString stringWithUTF8String:_info.productName.c_str()];
        productInfo.productDesc = [NSString stringWithUTF8String:_info.productDesc.c_str()];
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
        
        [m_delegate submitPaymentData:productInfo];
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
