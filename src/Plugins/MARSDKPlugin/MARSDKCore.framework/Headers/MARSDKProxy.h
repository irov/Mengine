//
//  MARSDKProxy.h
//  MARSDKCore
//
//  Created by js wu on 2021/6/25.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "MARHelper.h"

@class MARProductInfo;
@class MARDeviceInfo;
@class MARUserExtraData;

//  是否可以支付的回调
typedef void (^CheckPayResultCallback)(BOOL canPay);

static NSString * const MAR_EVENT_LOGIN_FAIL = @"marLoginFailed";    //  登录失败
static NSString * const MAR_EVENT_PAY_FAIL = @"marPayFailed";        //  支付失败
static NSString * const MAR_EVENT_PAY_CANCEL = @"marPayCanceled";    //  支付取消
static NSString * const MAR_EVENT_PAY_SUCCESS = @"marPaySuccess";    //  支付成功

//  MARSDK回调接口 游戏层
@protocol MARSDKDelegate <NSObject>

- (UIView *) GetView;
- (UIViewController *) GetViewController;

@optional

- (void) OnPlatformInit: (NSDictionary *)params;
- (void) OnUserLogin: (NSDictionary *)params;
- (void) OnUserLogout: (NSDictionary *)params;
- (void) OnPayPaid: (NSDictionary *)params;
- (void) OnEventWithCode: (int)code msg: (NSString*)msg;



/**
  isRealName: 1 : 已经实名；0:未实名
  age: 年龄
 */
-(void) OnRealName:(NSDictionary*)params;
-(void) OnEventCustom:(NSString*)eventName params:(NSDictionary*)params;

@end

//  MARSDK 回调接口 给其他插件使用
@protocol MARSDKEventDelegate <NSObject>

@optional

-(void) OnPlatformInit:(NSDictionary*)params;
-(void) OnUserLogin:(NSDictionary*)params;
-(void) OnUserLogout:(NSDictionary*)params;
-(void) OnPayPaid:(NSDictionary*)params;

// 检查是否可以支付
-(void) checkPayEnable:(MARProductInfo*)order resultCallback:(CheckPayResultCallback)callback;

/**
  isRealName: 1 : 已经实名；0:未实名
  age: 年龄
 */
-(void) OnRealName:(NSDictionary*)params;

@end

//  MARSDK的代理类，用于处理MARSDK的回调事件，数据统计，和MARServer的通讯
@interface MARProxy : NSObject

@property (strong, nonatomic) NSString* userID;
@property (strong, nonatomic) NSString* sdkUserID;
@property (strong, nonatomic) NSString* token;

@property (strong, nonatomic) id loginExtension;
@property BOOL initSuccess;
@property (strong, nonatomic) MARDeviceInfo* devInfo;
@property (strong, nonatomic) id<MARSDKEventDelegate> eventDelegate;

- (id) init;

- (UIView *) GetView;
- (UIViewController *) GetViewController;

- (void) doPlatformInit: (NSDictionary *)param;
- (void) doUserLogin: (NSDictionary *)param;
- (void) doUserLogout: (NSDictionary *)param;
- (void) doPayPaid: (NSDictionary *)param;
- (void) doPaySuccess:(NSDictionary*)param;
- (void) doRealName: (NSDictionary *)param;
- (void) doEventWithCode: (int)code msg: (NSString *)msg;
- (void) doCustomEvent: (NSString *)eventName params: (NSDictionary *)param;

// 提交设备统计数据
-(void) submitDeviceInfo:(MARDeviceInfo*)device responseHandler:(MARRequestCallback)handler;

// 提交用户统计数据
-(void) submitUserInfo:(MARUserExtraData*)userlog responseHandler:(MARRequestCallback)handler;


// 礼包兑换
-(void) exchangeGiftbag:(NSString *)redeemCode   responseHandler:(MARRequestCallback)handler;

 
// 账号验证方法
-(void) AccountValidate:(NSDictionary*)params responseHandler:(MARRequestCallback) handler;

//创建订单
-(void) requestOrder:(MARProductInfo*)params responseHandler:(MARRequestCallback)handler;

// 充值验证
-(void) PayValidate:(NSDictionary*)params responseHandler:(MARRequestCallback)handler;


// 手机端自动补单
-(void) automaticResendOrder:(NSDictionary*)params responseHandler:(MARRequestCallback) handler;



@end
