#pragma once

#import "Environment/Apple/AppleIncluder.h"

#import <pthread.h>

@interface iOSFirebaseCrashlyticsANRMonitor : NSObject

+ (instancetype)sharedInstance;

- (instancetype)init NS_UNAVAILABLE;

- (void)runANRMainWatcher;
- (void)runANRWatcher:(pthread_t)thread withName:(NSString *)threadName;

@property(atomic, assign) BOOL m_monitoringEnabled;

@end
