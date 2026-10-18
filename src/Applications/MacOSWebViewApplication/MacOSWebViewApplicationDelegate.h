#pragma once

#include "Interface/LoggerInterface.h"

#import "Platforms/AppleWebViewPlatform/AppleWebViewMCPApplicationInterface.h"

#import <AppKit/AppKit.h>

NS_ASSUME_NONNULL_BEGIN

@class AppleWebViewMCP;
@class WKWebView;
@class AppleLogRecordParam;

@interface MacOSWebViewApplicationDelegate : NSObject<NSApplicationDelegate, NSWindowDelegate, AppleWebViewMCPApplicationInterface>
{
@private
    Mengine::LoggerInterfacePtr m_logger;
    Mengine::LoggerInterfacePtr m_logCollector;
    NSMutableArray * m_records;

    AppleWebViewMCP * m_mcp;
}

@property (nonatomic, strong, nullable) NSTimer * m_runtimeTimer;
@property (nonatomic) CFTimeInterval m_previousTime;

#pragma mark - Application

- (BOOL)initializeApplication;
- (void)finalizeApplication;
- (void)stopApplication;

#pragma mark - WebView

- (nullable WKWebView *)browser;
- (NSDictionary *)webViewStatus;
- (void)reloadWebView;
- (void)captureWebView:(void (^)(NSData *, uint32_t, uint32_t))_completion;
- (void)evaluateWebView:(NSString *)_script completion:(void (^)(id _Nullable, NSError * _Nullable))_completion;

#pragma mark - Logging

- (NSArray *)readLogs;
- (void)eventLog:(AppleLogRecordParam *)_record;

@end

NS_ASSUME_NONNULL_END
