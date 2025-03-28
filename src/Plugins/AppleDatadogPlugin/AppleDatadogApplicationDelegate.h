#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginSessionIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginConfigDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"

@import DatadogObjc;

@interface AppleDatadogApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, iOSPluginSessionIdDelegateInterface, iOSPluginConfigDelegateInterface, iOSPluginLoggerDelegateInterface>

@property (atomic) BOOL m_enableDebugMessage;
@property (atomic) BOOL m_enableInfoMessage;

@property (nonatomic, strong) DDLogger * m_logger;

@end
