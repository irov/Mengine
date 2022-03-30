//
//  MARAnalytics.h
//  MARSDKCore
//
//  Created by js wu on 2021/6/25.
//

#import <Foundation/Foundation.h>

/// 游戏角色相关事件
typedef NS_ENUM(NSInteger, UserOperateCode)
{
    OP_CREATE_ROLE = 1,
    OP_ENTER_GAME,
    OP_LEVEL_UP,
    OP_EXIT
};

/// 游戏角色相关事件
typedef NS_ENUM(NSInteger, UserExtraDataType)
{
    TYPE_SELECT_SERVER = 1,
    TYPE_CREATE_ROLE,
    TYPE_ENTER_GAME,
    TYPE_LEVEL_UP,
    TYPE_EXIT_GAME
};

/// 设备信息
@interface MARDeviceInfo : NSObject

@property (strong, nonatomic)NSString* deviceID;
@property int appID;
@property int channelID;
@property (strong, nonatomic)NSString* mac;
@property (strong, nonatomic)NSString* deviceType;
@property int deviceOS;
@property (strong, nonatomic)NSString* deviceDpi;

-(NSDictionary*) toDict;
-(NSString*) toJsonString;

@end

/**
 *  用户扩展数据
 *  已经登录的角色相关数据
 *  现有渠道需要统计角色相关数据
 *  @author dayong
 */
@interface MARUserExtraData : NSObject

@property int dataType;
@property (strong, nonatomic)NSString* opType;
@property (strong, nonatomic)NSString* roleID;
@property (strong, nonatomic)NSString* roleName;
@property (strong, nonatomic)NSString* roleLevel;
@property int serverID;
@property (strong, nonatomic)NSString* serverName;
@property int moneyNum;
@property long roleCreateTime;
@property long roleLevelUpTime;

@property (strong, nonatomic) NSString* vip;
@property int roleGender;
@property (strong, nonatomic) NSString* professionID;
@property (strong, nonatomic) NSString* professionName;
@property (strong, nonatomic) NSString* power;
@property (strong, nonatomic) NSString* partyID;
@property (strong, nonatomic) NSString* partyName;
@property (strong, nonatomic) NSString* partyMasterID;
@property (strong, nonatomic) NSString* partyMasterName;

+(instancetype) dataFromJsonString:(NSString*)js;
-(instancetype) initWithDict:(NSDictionary*)dict;
-(NSDictionary *) toDict;
-(NSString *) toJsonString;

@end

@protocol IMARAnalytics

//开始关卡的时候，调用
-(void) startLevel:(NSString*)level;
//关卡失败时，调用
-(void) failLevel:(NSString*)level;
//关卡结束时，调用
-(void) finishLevel:(NSString*)level;

//开始任务的时候，调用
-(void) sartTask:(NSString*)task;

//任务失败时调用
-(void) failTask:(NSString*)task;

//任务完成时调用
-(void) finishTask:(NSString*)task;

//开始支付的时候调用
-(void) payRequest:(NSString*)orderID itemID:(NSString*)itemID money:(double)money currency:(NSString*)currency;

//支付成功的时候调用
-(void) pay:(NSString*)orderID money:(double)money num:(int)num;

//充值成功的时候调用
-(void) pay:(double)money coin:(int)coin source:(int)source;
-(void) pay:(double)money item:(NSString*)item num:(int)num price:(double)price source:(int)source;
//游戏中所有虚拟消费，比如用金币购买某个道具都使用 buy 方法
-(void) buy:(NSString*)item num:(int)num price:(double)price;

//消耗物品的时候，调用
-(void) use:(NSString*)item num:(int)num price:(double)price;

//额外获取虚拟币时，trigger 触发奖励的事件, 取值在 1~10 之间，“1”已经被预先定义为“系统奖励”， 2~10 需要在网站设置含义。
-(void) bonus:(NSString*)item num:(int)num price:(double)price trigger:(int)trigger;


//登录的时候调用
-(void) login:(NSString*)userID;

//进入游戏服务器时调用
-(void) enterServer:(NSString*)gameServer;

//登出的时候调用
-(void) logout;

//当玩家建立角色或者升级时，需调用此接口
-(void) levelup:(int)level;

@end

@interface MARAnalytics : NSObject<IMARAnalytics>

/// 统计接口实现
@property (strong, nonatomic) id analytics;

+(instancetype) sharedInstance;

@end


