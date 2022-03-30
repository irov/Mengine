//
//  MARPush.h
//  MARSDKCore
//
//  Created by js wu on 2021/6/28.
//

#ifndef MARPush_h
#define MARPush_h

#import <Foundation/Foundation.h>

//分享接口
@protocol IMARPush

//  执行通知
-(void)scheduleNotification:(NSString*)args;

//  开始推送
-(void)startPush;

//  停止推送
-(void)stopPush;

//  添加tag
-(void)addTags:(NSString*)tags;

//  删除tag
-(void)removeTags:(NSString*)tag;

//  添加别名
-(void)addAlias:(NSString*)alias;

//  删除别名
-(void)removeAlias:(NSString*)alias;

@end

@interface MARPush : NSObject<IMARPush>

/// 分享接口实现
@property (strong, nonatomic) NSObject<IMARPush>* push;

/// 获取MARSDKShare的单例
+(instancetype) sharedInstance;

@end

#endif /* MARPush_h */
