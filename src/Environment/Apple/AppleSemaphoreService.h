#pragma once

#include "Config/Config.h"

#import "AppleSemaphore.h"

#import <Foundation/Foundation.h>

@interface AppleSemaphoreService : NSObject

+ (instancetype _Nonnull) sharedInstance;

- (void)activateSemaphore:(NSString* _Nonnull)name;
- (void)deactivateSemaphore:(NSString* _Nonnull)name;
- (void)waitSemaphore:(NSString* _Nonnull)name listener:(void (^ _Nonnull)())cb;

@property (nonatomic, strong) NSMutableDictionary<NSString *, AppleSemaphore *> * _Nonnull m_semaphores;
                                                
@end
