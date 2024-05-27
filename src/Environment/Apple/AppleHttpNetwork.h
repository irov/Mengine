#import <Foundation/Foundation.h>

@interface MengineHttpRequestParam : NSObject

@property (nonatomic, strong) NSString * HTTP_URL;
@property (nonatomic, strong) NSString * HTTP_PROXY;
@property (nonatomic, strong) NSArray<NSString *> * HTTP_HEADERS;
@property (nonatomic, strong) NSString * HTTP_COOKIES;
@property (nonatomic, assign) int HTTP_TIMEOUT;

@end

@interface MengineHttpResponseParam : NSObject

@property (nonatomic, assign) int HTTP_RESPONSE_CODE;
@property (nonatomic, assign) int HTTP_CONTENT_LENGTH;
@property (nonatomic, strong) NSData * HTTP_CONTENT_DATA;
@property (nonatomic, strong) NSString * HTTP_ERROR_MESSAGE;

@end

@interface MengineNetwork : NSObject

+ (nullable MengineHttpResponseParam *)httpRequestPostMessage:(MengineHttpRequestParam *)request properties:(NSDictionary<NSString *, NSString *> *)properties;
+ (nullable MengineHttpResponseParam *)httpRequestHeaderData:(MengineHttpRequestParam *)request data:(NSData *)data;
+ (nullable MengineHttpResponseParam *)httpRequestGetMessage:(MengineHttpRequestParam *)request;
+ (nullable MengineHttpResponseParam *)httpRequestDeleteMessage:(MengineHttpRequestParam *)request;
+ (nullable MengineHttpResponseParam *)httpRequestGetAsset:(MengineHttpRequestParam *)request login:(NSString *)login password:(NSString *)password;

@end
