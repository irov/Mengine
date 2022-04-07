#include "AppleMARSDKService.h"

#include "Kernel/Logger.h"

#include "Config/Algorithm.h"

#import "MARSDKCore/MARSDKCore.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleMARSDKService, Mengine::AppleMARSDKService );
//////////////////////////////////////////////////////////////////////////
@interface AppleMARSDKCallbacks : NSObject<AppleMARSDKCallbacksProtocol>
//////////////////////////////////////////////////////////////////////////
@property Mengine::AppleMARSDKService * m_service;
//////////////////////////////////////////////////////////////////////////
- (instancetype _Nonnull)initWithCallback:(Mengine::AppleMARSDKService * _Nonnull) service;
- (void) onUserLogin:(NSDictionary* _Nullable) params;
- (void) onUserLogout:(NSDictionary* _Nullable) params;
- (void) onPayPaid:(NSDictionary* _Nullable) params;
//////////////////////////////////////////////////////////////////////////
@end
//////////////////////////////////////////////////////////////////////////
@implementation AppleMARSDKCallbacks
//////////////////////////////////////////////////////////////////////////
#pragma mark -
//////////////////////////////////////////////////////////////////////////
@synthesize m_service;
//////////////////////////////////////////////////////////////////////////
- (instancetype)initWithCallback:(Mengine::AppleMARSDKService * _Nonnull) service {
    m_service = service;
}
//////////////////////////////////////////////////////////////////////////
- (void) onUserLogin:(NSDictionary* _Nullable) params {
    m_service->onUserLogin();
}
//////////////////////////////////////////////////////////////////////////
- (void) onUserLogout:(NSDictionary* _Nullable) params {
    m_service->onUserLogout();
}
//////////////////////////////////////////////////////////////////////////
- (void) onPayPaid:(NSDictionary* _Nullable) params {
    m_service->onPayPaid();
}
//////////////////////////////////////////////////////////////////////////
@end
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
        id<AppleMARSDKCallbacksProtocol> * callbacks = (id<AppleMARSDKCallbacksProtocol> *)[[AppleMARSDKCallbacks alloc] initWithCallback:this];
        
        m_delegate = [[AppleMARSDKDelegate alloc] initWithCallbacks:callbacks];
        
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
        [m_delegate login];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKService::logout()
    {
        [m_delegate logout];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKService::switchAccount()
    {
        [m_delegate switchAccount];
        
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
        
        [m_delegate submitExtendedData:extraData];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::submitPayementData( const MARSDKProductInfo & _info )
    {
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
        
        [m_delegate submitPayementData:productInfo];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::onUserLogin()
    {
        if( m_provider == nullptr )
        {
            return;
        }
        
        m_provider->onUserLogin();
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::onUserLogout()
    {
        if( m_provider == nullptr )
        {
            return;
        }
        
        m_provider->onUserLogout();
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKService::onPayPaid()
    {
        if( m_provider == nullptr )
        {
            return;
        }
        
        m_provider->onPayPaid();
    }
    //////////////////////////////////////////////////////////////////////////
}
