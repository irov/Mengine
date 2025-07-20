#pragma once

#import "Environment/Apple/AppleIncluder.h"

#include "Kernel/LoggerLevel.h"

#import <Network/Network.h>

@interface iOSNetwork : NSObject

+ (instancetype _Nonnull)sharedInstance;

- (instancetype _Nonnull)init;

- (void)startMonitoring;
- (void)stopMonitoring;
- (BOOL)isNetworkAvailable;

@property (nonatomic) BOOL m_available;
@property (nonatomic) _Nonnull nw_path_monitor_t m_monitor;
@property (nonatomic) _Nonnull dispatch_queue_t m_queue;

@end
