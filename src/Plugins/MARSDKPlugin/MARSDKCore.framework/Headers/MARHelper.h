//
//  MARHelper.h
//  MARSDKCore
//
//  Created by js wu on 2021/6/25.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface MARHelper : NSObject

typedef void (^MARRequestCallback)(NSURLResponse* response, id data, NSError* connectionError);

// 获取单例
+(MARHelper*) sharedInstance;

///获取当前时间戳
-(NSString *)nowTimeStampSecs;

///md5
- (NSString *) stringMd5:(NSString*)src;


/// 获取mac地址
- (NSString *) macaddress;

/// 获取唯一设备码
- (NSString *) uniqueIdentifier;

/// NSDictionary转换为Http的URL参数
- (NSString*) encodeHttpParams:(NSDictionary*)params encode:(NSStringEncoding)encoding;

///针对java服务端， http encode将%20转为+
- (NSString *) formatHttpEncodeForJava: (NSString *) sign;

- (NSURL*) getMARServerUrl:(NSString*)relativePath;

/**
 * @brief 向MARServer发送http请求
 * @param httpParams 请求参数
 * @param requestPath 请求的路径
 * @param handler 回调函数
 * @param showprogress 是否显示Loading菊花
 */
-(void) sendHttpRequest:(NSDictionary *)httpParams toMARServer:(NSString *)requestPath responseHandler:(MARRequestCallback)handler showProgess:(Boolean)showprogress;
-(void) sendHttpgetRequest:(NSDictionary *)httpParams toMARServer:(NSString *)requestPath responseHandler:(MARRequestCallback)handler showProgess:(Boolean)showprogress;



@end

NS_ASSUME_NONNULL_END
