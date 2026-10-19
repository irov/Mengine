#pragma once

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol AppleWebViewMCPApplicationInterface <NSObject>

- (NSDictionary *)webViewStatus;
- (NSArray *)readLogs;

- (void)reloadWebView;
- (void)captureWebView:(void (^)(NSData *, uint32_t, uint32_t))_completion;
- (void)evaluateWebView:(NSString *)_script completion:(void (^)(id _Nullable, NSError * _Nullable))_completion;
- (void)stopApplication;

@end

NS_ASSUME_NONNULL_END
