//
//  MARShare.h
//  MARSDKCore
//
//  Created by js wu on 2021/6/28.
//

#import <Foundation/Foundation.h>

@interface MARShareInfo : NSObject

-(id)initWithDictionary:(NSDictionary*)dict;

-(NSString*) title;
-(NSString*) content;
-(NSString*) url;
-(NSString*) imgUrl;

-(id)valueForKey:(NSString *)key;
-(void)setValue:(id)value forKey:(NSString *)key;

@end

//分享接口
@protocol IMARShare

-(void)share:(MARShareInfo*)params;

@end

//分享组件
@protocol IMARShareComponent

-(NSArray*)supportPlatforms;
-(void)shareTo:(NSString*)platform shareParams:(MARShareInfo*)params;

@end

@interface MARShare : NSObject<IMARShare>

/// 分享接口实现
@property (strong, nonatomic) NSObject<IMARShare>* share;

/// 获取MARSDKShare的单例
+(instancetype) sharedInstance;

-(void) registerShareComponent:(NSObject<IMARShareComponent>*)shareComponent;

-(void)share:(MARShareInfo*)params;
-(NSArray*)supportPlatforms;
-(void)shareTo:(NSString*)platform shareParams:(MARShareInfo*)params;

@end
