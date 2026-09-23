#pragma once

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/Apple/AppleSemaphoreListenerInterface.h"

@interface AppleSemaphoreListenerNSProxy : NSObject

- (instancetype _Nonnull) initWithListener:(const Mengine::AppleSemaphoreListenerInterfacePtr &)_proxy;

- (void)invoke:(id _Nullable)value;

@property (nonatomic) Mengine::AppleSemaphoreListenerInterfacePtr m_proxy;

@end
