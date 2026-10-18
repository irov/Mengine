#pragma once

#include "Interface/LoggerInterface.h"

#import "Platforms/AppleWebViewPlatform/AppleWebViewMCPApplicationInterface.h"

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@class AppleWebViewMCP;
@class AppleLogRecordParam;
@class WKWebView;

@interface iOSWebViewApplicationDelegate : UIResponder<UIApplicationDelegate, AppleWebViewMCPApplicationInterface>
{
@private
    Mengine::LoggerInterfacePtr m_logger;
    Mengine::LoggerInterfacePtr m_logCollector;
    NSMutableArray * m_records;

    AppleWebViewMCP * m_mcp;
}

@property (nonatomic, strong, nullable) NSTimer * m_runtimeTimer;
@property (nonatomic) CFTimeInterval m_previousTime;

- (nullable WKWebView *)browser;
- (NSDictionary *)webViewStatus;
- (NSArray *)readLogs;

- (BOOL)initializeApplication;
- (void)finalizeApplication;

- (BOOL)connectScene:(UIWindowScene *)_scene;
- (void)disconnectScene;
- (void)resumeRuntime;
- (void)pauseRuntime;

- (void)eventLog:(AppleLogRecordParam *)_record;

- (void)reloadWebView;
- (void)captureWebView:(void (^)(NSData *, uint32_t, uint32_t))_completion;
- (void)evaluateWebView:(NSString *)_script completion:(void (^)(id _Nullable, NSError * _Nullable))_completion;
- (void)stopApplication;

@end

NS_ASSUME_NONNULL_END
