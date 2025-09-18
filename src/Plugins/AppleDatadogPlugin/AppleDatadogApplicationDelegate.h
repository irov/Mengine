#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginUserIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginConfigDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"
#import "Environment/iOS/iOSPluginTransparencyConsentDelegateInterface.h"

@class DDLogger;

@interface AppleDatadogApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, iOSPluginUserIdDelegateInterface, iOSPluginConfigDelegateInterface, iOSPluginLoggerDelegateInterface, iOSPluginTransparencyConsentDelegateInterface>

@property (atomic) BOOL m_enableDebugMessage;
@property (atomic) BOOL m_enableInfoMessage;

@property (nonatomic, strong) DDLogger * m_logger;

@end
