//
//  MARSDK.h
//  MARSDKCore
//
//  Created by js wu on 2021/6/28.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import "MARSDKProxy.h"
#import "MARAnalytics.h"
#import "MARUser.h"
#import "MARPay.h"


#ifdef __cplusplus
#define MARSDK_EXTERN        extern "C" __attribute__((visibility ("default")))
#else
#define MARSDK_EXTERN        extern __attribute__((visibility ("default")))
#endif
//状态码
static int const MAR_CODE_LOGIN_FAILED = 1;           //登录失败
static int const MAR_CODE_LOGIN_CANCEL = 2;           //登录取消
static int const MAR_CODE_PAY_SUCCESS = 3;            //支付成功
static int const MAR_CODE_PAY_FAILED = 4;             //支付失败
static int const MAR_CODE_PAY_CANCEL = 5;             //支付取消
static int const MAR_CODE_PAY_UNKNOWN = 6;            //支付结果未知
static int const MAR_CODE_INIT_FAILED = 7;            //初始化失败
static int const MAR_CODE_INIT_SUCCESS = 8;           //初始化成功
static int const MAR_CODE_SWITCH_ACCOUNT_FAILED = 9;  //切换账号失败

// MARSDK的核心类
// 负责插件管理和事件分发
@interface MARSDK : NSObject

/// MARSDK的参数
@property (nonatomic, copy) NSDictionary* sdkParams;
/// APP支持的屏幕方向
@property UIInterfaceOrientationMask supportedOrientations;
/// 渠道号
@property (readonly) int channelId;
// 渠道名
@property (readonly) NSString* channelName;

// 委托回调对象
@property id<MARSDKDelegate> delegate;

// MARServer代理
@property (strong, nonatomic) MARProxy* proxy;

/// 默认的账号接口实现
@property (strong, nonatomic) id defaultUser;
/// 默认的支付接口实现
@property (strong, nonatomic) id defaultPay;

/// 获取MARSDK的单例
+(MARSDK*) sharedInstance;

/// 添加插件
-(void) registerPlugin:(NSObject*)plugin;

/**
 * @brief 查找接口对象
 * @param name 插件的名称
 * @param protocolType 接口的类型
 * @return 如果指定了protocolType，就返回对应接口的对象， 否则返回插件对象
 */
-(NSObject*) getInterfaceByName:(NSString*)name andProtocol:(Protocol *)protocolType;

-(NSArray*) plugins;

-(void) initWithParams:(NSDictionary*)params;

-(BOOL) isInitCompleted;

-(void) setupWithParams:(NSDictionary*)params;
-(void) submitExtraData:(MARUserExtraData*)data;
//    gameId：游戏兑换码   
-(void)exchangeGiftbag:(NSString *)gameId;
-(id) customCall:(NSString*)method withParams:(NSDictionary*)params;

#pragma mark -- 帐号登录接口 --
-(void) login;
-(void) logout;
-(void) switchAccount;
-(BOOL) hasAccountCenter;

-(void) loginCustom:(NSString*)customData;
-(void) showAccountCenter;

- (BOOL) hasRealNameQuery;
- (BOOL) hasRealNameRegister;
- (void) realNameQuery;
- (void) realNameRegister;

-(BOOL) exit;

#pragma mark -- 支付充值接口 --
-(void) pay:(MARProductInfo*) profuctInfo;
-(void) finishTransaction:(NSString*)transactionIdentifier;

// 道具到账
-(void)propComplete:(NSString *)orderId responseHandler:(MARRequestCallback) handler;
@end
