//
//  MARAction.h
//  MARSDKCore
//
//  Created by js wu on 2021/6/24.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

static NSString * const MAR_ACTION_KEY_SUCCESS = @"isSuccess";           //操作是否成功
static NSString * const MAR_ACTION_KEY_METHOD = @"method";               //登录方式
static NSString * const MAR_ACTION_KEY_USERID = @"userID";               //用户ID
static NSString * const MAR_ACTION_KEY_SERVERID = @"serverID";           //服务器ID
static NSString * const MAR_ACTION_KEY_SERVER_NAME = @"serverName";      //服务器名称
static NSString * const MAR_ACTION_KEY_ROLEID = @"roleID";               //角色ID
static NSString * const MAR_ACTION_KEY_ROLE_NAME = @"roleName";          //角色名称
static NSString * const MAR_ACTION_KEY_ROLE_LEVEL = @"roleLevel";        //角色等级
static NSString * const MAR_ACTION_KEY_PRODUCT_TYPE = @"productType";    //商品类型，比如礼包，首冲等
static NSString * const MAR_ACTION_KEY_PRODUCTID = @"productID";         //商品ID
static NSString * const MAR_ACTION_KEY_PRODUCT_NAME = @"productName";    //商品名称
static NSString * const MAR_ACTION_KEY_PRODUCT_NUM = @"productNum";      //商品数量
static NSString * const MAR_ACTION_KEY_PAY_TYPE = @"payType";            //支付方式
static NSString * const MAR_ACTION_KEY_CURRENCY = @"currency";           //货币单位
static NSString * const MAR_ACTION_KEY_PRICE = @"price";                 //金额，单位分
static NSString * const MAR_ACTION_KEY_ORDERID = @"orderID";             //订单号
static NSString * const MAR_ACTION_KEY_BUY_TYPE = @"buyType";            //购买方式，比如礼包，基金等
static NSString * const MAR_ACTION_KEY_TARGET_ID = @"targetID";          //购买项的ID
static NSString * const MAR_ACTION_KEY_TARGET_NAME = @"targetName";      //购买项的名称
static NSString * const MAR_ACTION_KEY_TARGET_NUM = @"targetNum";        //购买数量
static NSString * const MAR_ACTION_KEY_V_CURRENCY = @"isVirtualCurrency";//是否虚拟货币
static NSString * const MAR_ACTION_KEY_V_CUREENCY_NAME = @"virtualCurrencyName"; //虚拟货币的名称
static NSString * const MAR_ACTION_KEY_COST = @"costAmount";             //消耗的虚拟币个数
static NSString * const MAR_ACTION_KEY_TASK_TYPE = @"taskType";          //任务类型
static NSString * const MAR_ACTION_KEY_TASKID = @"taskID";               //任务ID
static NSString * const MAR_ACTION_KEY_TASK_NAME = @"taskName";          //任务名称
static NSString * const MAR_ACTION_KEY_TASK_LEVEL = @"taskLevel";        //任务等级
static NSString * const MAR_ACTION_KEY_TASK_DESC = @"taskDesc";          //任务描述

//  行为统计插件接口
@protocol IMARAction

//  注册，一般是SDK的注册
-(void) userRegister:(NSDictionary*) params;

//  登录，一般是SDK的登录
-(void) login:(NSDictionary*) params;

//  创建角色
-(void) createRole:(NSDictionary*) params;

//  进入游戏
-(void) enterGame:(NSDictionary*) params;

//  等级提升
-(void) levelUp:(NSDictionary*) params;

//  充值
-(void) purchase:(NSDictionary*) params;

//  购买，游戏中用钻石或者元宝等购买其他道具
-(void) buy:(NSDictionary*) params;

//  关卡，任务，新手引导等
-(void) task:(NSDictionary*) params;

//  自定义上报
-(void) customEventWithName:(NSString*) eventName params:(NSDictionary*) params;




@end

@interface MARAction : NSObject<IMARAction>

/// 行为统计接口实现
@property (strong, nonatomic) NSObject<IMARAction>* action;

///  获取MARAction的单例
+(instancetype) sharedInstance;

@end

NS_ASSUME_NONNULL_END
