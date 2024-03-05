#pragma once

#import "Environment/Apple/AppleSemaphoreListenerNSProxy.h"

#import <Foundation/Foundation.h>

@interface AppleSemaphore : NSObject

- (instancetype _Nonnull) initWithActivated:(BOOL)activated;

- (BOOL)isActivated;
- (NSMutableArray<AppleSemaphoreListenerNSProxy *> * _Nonnull)activate;
- (void)addListener:(AppleSemaphoreListenerNSProxy * _Nonnull)listener;

@property (nonatomic) BOOL m_activated;
@property (nonatomic, strong) NSMutableArray<AppleSemaphoreListenerNSProxy *> * _Nullable m_listeners;

@end
