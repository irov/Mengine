#import <Foundation/Foundation.h>

@interface MengineHttpRequestParam : NSObject

@property (nonatomic, strong) NSString * _Nonnull HTTP_URL;
@property (nonatomic, strong) NSString * _Nullable HTTP_PROXY;
@property (nonatomic, strong) NSArray<NSString *> * _Nullable HTTP_HEADERS;
@property (nonatomic, strong) NSString * _Nullable HTTP_COOKIES;
@property (nonatomic, assign) int HTTP_TIMEOUT;

@end

@interface MengineHttpResponseParam : NSObject

@property (nonatomic, assign) int HTTP_RESPONSE_CODE;
@property (nonatomic, assign) int HTTP_CONTENT_LENGTH;
@property (nonatomic, strong) NSData * _Nullable HTTP_CONTENT_DATA;
@property (nonatomic, strong) NSString * _Nullable HTTP_ERROR_MESSAGE;
@property (nonatomic, assign) NSInteger HTTP_ERROR_CODE;

@end

@interface MengineHttpNetwork : NSObject

+ (nullable MengineHttpResponseParam *)httpRequestPostMessage:(MengineHttpRequestParam * _Nonnull)request properties:(NSDictionary<NSString *, NSString *> * _Nonnull)properties;
+ (nullable MengineHttpResponseParam *)httpRequestHeaderData:(MengineHttpRequestParam * _Nonnull)request data:(NSData * _Nonnull)data;
+ (nullable MengineHttpResponseParam *)httpRequestGetMessage:(MengineHttpRequestParam * _Nonnull)request;
+ (nullable MengineHttpResponseParam *)httpRequestDeleteMessage:(MengineHttpRequestParam * _Nonnull)request;
+ (nullable MengineHttpResponseParam *)httpRequestGetAsset:(MengineHttpRequestParam * _Nonnull)request login:(NSString * _Nullable)login password:(NSString * _Nullable)password;

@end
