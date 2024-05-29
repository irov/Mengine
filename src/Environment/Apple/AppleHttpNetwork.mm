#import "AppleHttpNetwork.h"

@implementation MengineHttpRequestParam

@end

@implementation MengineHttpResponseParam

@end

@implementation MengineNetwork

+ (void)setTimeout:(NSMutableURLRequest *)request timeout:(int)timeout {
    if (timeout != -1) {
        request.timeoutInterval = timeout;
    }
}

+ (void)setBasicAuthorization:(NSMutableURLRequest *)request login:(NSString *)login password:(NSString *)password {
    NSString * userCredentials = [NSString stringWithFormat:@"%@:%@", login, password];
    NSData * data = [userCredentials dataUsingEncoding:NSUTF8StringEncoding];
    NSString * base64 = [data base64EncodedStringWithOptions:0];
    NSString * authHeader = [NSString stringWithFormat:@"Basic %@", base64];
    [request setValue:authHeader forHTTPHeaderField:@"Authorization"];
}

+ (void)setData:(NSMutableURLRequest *)request data:(NSData *)data {
    request.HTTPBody = data;
}

+ (void)setHeaders:(NSMutableURLRequest *)request headers:(NSArray<NSString *> *)headers {
    for (NSString * header in headers) {
        NSArray * parts = [header componentsSeparatedByString:@":"];
        if (parts.count == 2) {
            [request setValue:[parts[1] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]] forHTTPHeaderField:parts[0]];
        }
    }
}

+ (void)setMultipartFormData:(NSMutableURLRequest *)request properties:(NSDictionary<NSString *, NSString *> *)properties {
    NSString * boundary = [[NSUUID UUID] UUIDString];
    NSString * contentType = [NSString stringWithFormat:@"multipart/form-data; boundary=%@", boundary];
    [request setValue:contentType forHTTPHeaderField:@"Content-Type"];
    
    NSMutableData * body = [NSMutableData data];
    [properties enumerateKeysAndObjectsUsingBlock:^(NSString *key, NSString *value, BOOL *stop) {
        [body appendData:[[NSString stringWithFormat:@"--%@\r\n", boundary] dataUsingEncoding:NSUTF8StringEncoding]];
        [body appendData:[[NSString stringWithFormat:@"Content-Disposition: form-data; name=\"%@\"\r\n\r\n", key] dataUsingEncoding:NSUTF8StringEncoding]];
        [body appendData:[[NSString stringWithFormat:@"%@\r\n", value] dataUsingEncoding:NSUTF8StringEncoding]];
    }];
    [body appendData:[[NSString stringWithFormat:@"--%@--\r\n", boundary] dataUsingEncoding:NSUTF8StringEncoding]];
    
    request.HTTPBody = body;
}

+ (MengineHttpResponseParam *)makeResponse:(NSHTTPURLResponse *)response data:(NSData *)data error:(NSError *)error {
    MengineHttpResponseParam * httpResponse = [[MengineHttpResponseParam alloc] init];
    
    if (response != nil) {
        httpResponse.HTTP_RESPONSE_CODE = (int)response.statusCode;
    } else {
        httpResponse.HTTP_RESPONSE_CODE = 0;
    }
    
    if (error != nil) {
        httpResponse.HTTP_ERROR_MESSAGE = error.domain;
    } else {
        httpResponse.HTTP_ERROR_MESSAGE = nil;
    }
    
    if (data != nil) {
        httpResponse.HTTP_CONTENT_DATA = data;
        httpResponse.HTTP_CONTENT_LENGTH = (int)data.length;
    } else {
        httpResponse.HTTP_CONTENT_DATA = nil;
        httpResponse.HTTP_CONTENT_LENGTH = 0;
    }
    
    return httpResponse;
}

+ (nullable MengineHttpResponseParam *)httpRequestPostMessage:(MengineHttpRequestParam *)request properties:(NSDictionary<NSString *, NSString *> *)properties {
    @try {
        NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:request.HTTP_URL]];
        urlRequest.HTTPMethod = @"POST";
        [self setTimeout:urlRequest timeout:request.HTTP_TIMEOUT];
        [self setHeaders:urlRequest headers:request.HTTP_HEADERS];
        [self setMultipartFormData:urlRequest properties:properties];
        
        __block MengineHttpResponseParam *responseParam = nil;
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
        
        NSURLSessionDataTask * dataTask = [[NSURLSession sharedSession] dataTaskWithRequest:urlRequest completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
            NSHTTPURLResponse * httpResponse = (NSHTTPURLResponse *)response;
            responseParam = [self makeResponse:httpResponse data:data error:error];
            dispatch_semaphore_signal(semaphore);
        }];
        
        [dataTask resume];
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        
        return responseParam;
    } @catch (NSException *exception) {
        NSLog(@"[HTTP] httpRequestPostMessage caught an exception: %@", exception.reason);
    }
    
    return nil;
}

+ (nullable MengineHttpResponseParam *)httpRequestHeaderData:(MengineHttpRequestParam *)request data:(NSData *)data {
    @try {
        NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:request.HTTP_URL]];
        urlRequest.HTTPMethod = @"POST";
        [self setTimeout:urlRequest timeout:request.HTTP_TIMEOUT];
        [self setHeaders:urlRequest headers:request.HTTP_HEADERS];
        [self setData:urlRequest data:data];
        
        __block MengineHttpResponseParam * responseParam = nil;
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
        
        NSURLSessionDataTask * dataTask = [[NSURLSession sharedSession] dataTaskWithRequest:urlRequest completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
            NSHTTPURLResponse * httpResponse = (NSHTTPURLResponse *)response;
            responseParam = [self makeResponse:httpResponse data:data error:error];
            dispatch_semaphore_signal(semaphore);
        }];
        
        [dataTask resume];
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        
        return responseParam;
    } @catch (NSException *exception) {
        NSLog(@"[HTTP] httpRequestHeaderData caught an exception: %@", exception.reason);
    }
    
    return nil;
}

+ (nullable MengineHttpResponseParam *)httpRequestGetMessage:(MengineHttpRequestParam *)request {
    @try {
        NSMutableURLRequest * urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:request.HTTP_URL]];
        urlRequest.HTTPMethod = @"GET";
        [self setTimeout:urlRequest timeout:request.HTTP_TIMEOUT];
        [self setHeaders:urlRequest headers:request.HTTP_HEADERS];
        
        __block MengineHttpResponseParam *responseParam = nil;
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
        
        NSURLSessionDataTask * dataTask = [[NSURLSession sharedSession] dataTaskWithRequest:urlRequest completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
            NSHTTPURLResponse * httpResponse = (NSHTTPURLResponse *)response;
            responseParam = [self makeResponse:httpResponse data:data error:error];
            dispatch_semaphore_signal(semaphore);
        }];
        
        [dataTask resume];
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        
        return responseParam;
    } @catch (NSException *exception) {
        NSLog(@"[HTTP] httpRequestGetMessage caught an exception: %@", exception.reason);
    }
    
    return nil;
}

+ (nullable MengineHttpResponseParam *)httpRequestDeleteMessage:(MengineHttpRequestParam *)request {
    @try {
        NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:request.HTTP_URL]];
        urlRequest.HTTPMethod = @"DELETE";
        [self setTimeout:urlRequest timeout:request.HTTP_TIMEOUT];
        [self setHeaders:urlRequest headers:request.HTTP_HEADERS];
        
        __block MengineHttpResponseParam * responseParam = nil;
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
        
        NSURLSessionDataTask * dataTask = [[NSURLSession sharedSession] dataTaskWithRequest:urlRequest completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
            NSHTTPURLResponse * httpResponse = (NSHTTPURLResponse *)response;
            responseParam = [self makeResponse:httpResponse data:data error:error];
            dispatch_semaphore_signal(semaphore);
        }];
        
        [dataTask resume];
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        
        return responseParam;
    } @catch (NSException *exception) {
        NSLog(@"[HTTP] httpRequestDeleteMessage caught an exception: %@", exception.reason);
    }
    
    return nil;
}

+ (nullable MengineHttpResponseParam *)httpRequestGetAsset:(MengineHttpRequestParam *)request login:(NSString *)login password:(NSString *)password {
    @try {
        NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:request.HTTP_URL]];
        urlRequest.HTTPMethod = @"GET";
        [self setTimeout:urlRequest timeout:request.HTTP_TIMEOUT];
        [self setBasicAuthorization:urlRequest login:login password:password];
        [self setHeaders:urlRequest headers:request.HTTP_HEADERS];
        
        __block MengineHttpResponseParam * responseParam = nil;
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
        
        NSURLSessionDataTask * dataTask = [[NSURLSession sharedSession] dataTaskWithRequest:urlRequest completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
            NSHTTPURLResponse * httpResponse = (NSHTTPURLResponse *)response;
            responseParam = [self makeResponse:httpResponse data:data error:error];
            dispatch_semaphore_signal(semaphore);
        }];
        
        [dataTask resume];
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        
        return responseParam;
    } @catch (NSException *exception) {
        NSLog(@"[HTTP] httpRequestGetAsset caught an exception: %@", exception.reason);
    }
    
    return nil;
}

@end
