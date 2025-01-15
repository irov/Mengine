#pragma once

#import <Foundation/Foundation.h>

@interface AppleHttpRequestParam : NSObject

@property (nonatomic, strong) NSString * _Nonnull HTTP_URL;
@property (nonatomic, strong) NSString * _Nullable HTTP_PROXY;
@property (nonatomic, strong) NSArray<NSString *> * _Nullable HTTP_HEADERS;
@property (nonatomic, strong) NSString * _Nullable HTTP_COOKIES;
@property (nonatomic, assign) int HTTP_TIMEOUT;

@end

@interface AppleHttpResponseParam : NSObject

@property (nonatomic, assign) int HTTP_RESPONSE_CODE;
@property (nonatomic, assign) int HTTP_CONTENT_LENGTH;
@property (nonatomic, strong) NSData * _Nullable HTTP_CONTENT_DATA;
@property (nonatomic, strong) NSString * _Nullable HTTP_ERROR_MESSAGE;
@property (nonatomic, assign) NSInteger HTTP_ERROR_CODE;

@end

@interface AppleHttpNetwork : NSObject

+ (void)httpRequestSetCacheWithMemoryCapacity:(NSUInteger)memoryCapacity diskCapacity:(NSUInteger)diskCapacity diskPath:(NSString * _Nonnull)diskPath;
+ (void)httpRequestClearCache;

+ (nullable AppleHttpResponseParam *)httpRequestPing:(AppleHttpRequestParam * _Nonnull)request;
+ (nullable AppleHttpResponseParam *)httpRequestPostMessage:(AppleHttpRequestParam * _Nonnull)request properties:(NSDictionary<NSString *, NSString *> * _Nonnull)properties;
+ (nullable AppleHttpResponseParam *)httpRequestHeaderData:(AppleHttpRequestParam * _Nonnull)request data:(NSData * _Nonnull)data;
+ (nullable AppleHttpResponseParam *)httpRequestGetMessage:(AppleHttpRequestParam * _Nonnull)request;
+ (nullable AppleHttpResponseParam *)httpRequestDeleteMessage:(AppleHttpRequestParam * _Nonnull)request;
+ (nullable AppleHttpResponseParam *)httpRequestGetAsset:(AppleHttpRequestParam * _Nonnull)request login:(NSString * _Nullable)login password:(NSString * _Nullable)password;

@end
