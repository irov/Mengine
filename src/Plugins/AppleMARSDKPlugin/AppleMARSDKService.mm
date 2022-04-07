#include "AppleMARSDKService.h"

#include "Environment/Apple/AppleUtils.h"

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
: m_appleMARSDKNative( nil )
{
}
//////////////////////////////////////////////////////////////////////////
AppleMARSDKService::~AppleMARSDKService()
{
}
//////////////////////////////////////////////////////////////////////////
bool AppleMARSDKService::_initializeService()
{
    m_appleMARSDKNative = [[AppleMARSDKDelegate alloc] initWithLogIn:^ (NSDictionary * _Nullable params) {
        if (params) {
            return;
        }
    } logOut:^ (NSDictionary * _Nullable params) {
        if (params) {
            return;
        }
    } onPayPaid:^ (NSDictionary * _Nullable params) {
        if (params) {
            return;
        }
    }];
    
    return true;
}
////////////////////////////////////////////////////////////////////////
void AppleMARSDKService::_finalizeService()
{
    
    [m_appleMARSDKNative release];
    m_appleMARSDKNative = nil;
}
//////////////////////////////////////////////////////////////////////
bool AppleMARSDKService::login()
{
    [m_appleMARSDKNative login];
    
    return true;
}
//////////////////////////////////////////////////////////////////////////
bool AppleMARSDKService::logout()
{
    [m_appleMARSDKNative logout];
    
    return true;
}
//////////////////////////////////////////////////////////////////////////
bool AppleMARSDKService::switchAccount()
{
    [m_appleMARSDKNative switchAccount];
    
    return true;
}
//////////////////////////////////////////////////////////////////////////
void AppleMARSDKService::submitExtendedData(const Mengine::ExtraData& extradata){
    MARUserExtraData* extraData = [[MARUserExtraData alloc] init];
    
    extraData.dataType = extradata.dataType;
    extraData.opType = [NSString stringWithUTF8String: extradata.opType.c_str()];
    extraData.roleID = [NSString stringWithUTF8String: extradata.roleID.c_str()];
    extraData.roleName = [NSString stringWithUTF8String: extradata.roleName.c_str()];
    extraData.roleLevel = [NSString stringWithUTF8String: extradata.roleLevel.c_str()];
    extraData.serverID = extradata.serverId;
    extraData.serverName = [NSString stringWithUTF8String: extradata.serverName.c_str()];
    extraData.moneyNum = extradata.moneyNum;
    extraData.roleCreateTime = extradata.roleCreateTime;
    extraData.roleLevelUpTime = extradata.roleLevelUpTime;
    
    extraData.vip = [NSString stringWithUTF8String: extradata.vip.c_str()];
    
    extraData.roleGender = extradata.roleGender;
    extraData.professionID = [NSString stringWithUTF8String: extradata.professionID.c_str()];
    extraData.professionName = [NSString stringWithUTF8String: extradata.professionName.c_str()];
    extraData.power = [NSString stringWithUTF8String: extradata.power.c_str()];
    extraData.partyID = [NSString stringWithUTF8String: extradata.partyID.c_str()];
    extraData.partyName = [NSString stringWithUTF8String: extradata.partyName.c_str()];
    extraData.partyMasterID = [NSString stringWithUTF8String: extradata.partyMasterID.c_str()];
    extraData.partyMasterName = [NSString stringWithUTF8String: extradata.partyMasterName.c_str()];
    
    [m_appleMARSDKNative submitExtendedData:extraData];
}
//////////////////////////////////////////////////////////////////////////
void AppleMARSDKService::submitPayementData(const Mengine::ProductInfo& product){
    MARProductInfo* productInfo = [[MARProductInfo alloc] init];
    productInfo.orderID = [NSString stringWithUTF8String:product.orderID.c_str()];
    productInfo.productId = [NSString stringWithUTF8String:product.productId.c_str()];
    productInfo.productName = [NSString stringWithUTF8String:product.productName.c_str()];
    productInfo.productDesc = [NSString stringWithUTF8String:product.productDesc.c_str()];
    productInfo.price = [NSNumber numberWithInt:product.price];
    productInfo.buyNum = product.buyNum;
    productInfo.coinNum = product.coinNum;
    
    productInfo.roleId = [NSString stringWithUTF8String: product.roleId.c_str()];
    productInfo.roleName = [NSString stringWithUTF8String: product.roleName.c_str()];
    productInfo.roleLevel = [NSString stringWithUTF8String: product.roleLevel.c_str()];
    productInfo.vip = [NSString stringWithUTF8String: product.vip.c_str()];
    productInfo.serverId = [NSString stringWithUTF8String: product.serverId.c_str()];
    productInfo.serverName = [NSString stringWithUTF8String: product.serverName.c_str()];
    productInfo.notifyUrl = [NSString stringWithUTF8String: product.notifyUrl.c_str()];
    
    
    [m_appleMARSDKNative submitPayementData:productInfo];
}
//////////////////////////////////////////////////////////////////////////
}
