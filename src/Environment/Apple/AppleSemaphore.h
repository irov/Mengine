#pragma once

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/Apple/AppleSemaphoreListenerNSProxy.h"

@interface AppleSemaphore : NSObject

- (instancetype _Nonnull) initWithActivated:(BOOL)activated value:(id _Nullable)value;

- (BOOL)isActivated;
- (id _Nullable)getValue;
- (NSMutableArray<AppleSemaphoreListenerNSProxy *> * _Nonnull)activate:(id _Nullable)value;
- (void)addListener:(AppleSemaphoreListenerNSProxy * _Nonnull)listener;

@property (nonatomic) BOOL m_activated;
@property (nonatomic, strong) id _Nullable m_value;
@property (nonatomic, strong) NSMutableArray<AppleSemaphoreListenerNSProxy *> * _Nullable m_listeners;

@end
