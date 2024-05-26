#pragma once

#import "Environment/Apple/AppleSemaphoreListenerInterface.h"

#import <Foundation/Foundation.h>

@interface AppleSemaphoreListenerNSProxy : NSObject

- (instancetype _Nonnull) initWithListener:(const Mengine::AppleSemaphoreListenerInterfacePtr &)_proxy;

- (void) invoke;

@property (nonatomic) Mengine::AppleSemaphoreListenerInterfacePtr m_proxy;

@end
