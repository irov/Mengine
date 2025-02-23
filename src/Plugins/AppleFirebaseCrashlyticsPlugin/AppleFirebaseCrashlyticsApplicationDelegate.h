#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginSessionIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"

#import "AppleFirebaseCrashlyticsANRMonitor.h"

@interface AppleFirebaseCrashlyticsApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, iOSPluginLoggerDelegateInterface, iOSPluginSessionIdDelegateInterface>

+ (AppleFirebaseCrashlyticsApplicationDelegate *) sharedInstance;

@property (nonatomic) AppleFirebaseCrashlyticsANRMonitor * m_ANRMonitor;

@end
