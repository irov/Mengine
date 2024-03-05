#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface AppleSemaphore : NSObject

- (instancetype _Nonnull) initWithActivated:(BOOL)activated;

- (BOOL)isActivated;
- (NSMutableArray<void (^)()>* _Nonnull)activate;
- (void)addListener:(void (^ _Nonnull)())listener;

@property (nonatomic) BOOL m_activated;
@property (nonatomic, strong) NSMutableArray<void (^)()> * _Nullable m_listeners;

@end
