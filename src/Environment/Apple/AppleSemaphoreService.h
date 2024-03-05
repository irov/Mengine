#pragma once

#include "Config/Config.h"

#import "AppleSemaphore.h"

#import <Foundation/Foundation.h>

@interface AppleSemaphoreService : NSObject

+ (instancetype _Nonnull) sharedInstance;

- (void)activateSemaphore:(NSString* _Nonnull)name;
- (void)deactivateSemaphore:(NSString* _Nonnull)name;
- (void)waitSemaphore:(NSString* _Nonnull)name withListener:(AppleSemaphoreListenerNSProxy * _Nonnull)listener;

@property (nonatomic, strong) NSMutableDictionary<NSString *, AppleSemaphore *> * _Nonnull m_semaphores;
                                                
@end
