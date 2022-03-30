//
//  MARPay.h
//  MARSDKCore
//
//  Created by js wu on 2021/6/25.
//

#import <Foundation/Foundation.h>

@interface MARProductInfo : NSObject

@property (strong, nonatomic)NSString* orderID;
@property (strong, nonatomic)NSString* productId;
@property (strong, nonatomic)NSString* productName;
@property (strong, nonatomic)NSString* productDesc;

@property (nonatomic,assign) int payType;


@property (strong, nonatomic)NSNumber* price;
@property NSInteger buyNum;
@property NSInteger coinNum;

@property (strong, nonatomic)NSString* roleId;
@property (strong, nonatomic)NSString* roleName;
@property (strong, nonatomic)NSString* roleLevel;
@property (strong, nonatomic)NSString* vip;
@property (strong, nonatomic)NSString* serverId;
@property (strong, nonatomic)NSString* serverName;
@property (strong, nonatomic)NSString* notifyUrl;

@property (strong, nonatomic)id extension;

+(instancetype) productFromJsonString:(NSString*)js;
-(instancetype) initWithDict:(NSDictionary*)dict;
-(NSString*) toJsonString;
-(NSDictionary*) toDict;

@end

//MARPay 应用内购接口
@protocol IMARPay

-(void) pay:(MARProductInfo*) profuctInfo;

@optional
-(void) closeIAP;
-(void) finishTransactionId:(NSString*)transactionId;

@end

__attribute__ ((deprecated))
@protocol MARPay
@end

