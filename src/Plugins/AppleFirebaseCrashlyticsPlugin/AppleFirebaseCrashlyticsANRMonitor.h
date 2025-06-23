#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface AppleFirebaseCrashlyticsANRMonitor : NSObject

- (void)runANRMainWatcher;
- (void)runANRWatcher:(thread_act_t)thread;

@property(atomic, assign) BOOL m_monitoringEnabled = YES;

@end