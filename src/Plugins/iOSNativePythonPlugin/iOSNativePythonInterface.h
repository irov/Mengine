#pragma once

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/Apple/AppleSemaphoreListenerInterface.h"
#include "Environment/Python/PythonIncluder.h"

@protocol iOSNativePythonInterface <NSObject>

+ (instancetype _Nonnull)sharedInstance;

- (pybind::object)addAppleCallback:(NSString * _Nullable)plugin
                             method:(NSString * _Nullable)method
                           callback:(const pybind::object &)cb
                               args:(const pybind::args &)args;
- (void)removeAppleCallback:(NSString * _Nullable)plugin
                     method:(NSString * _Nullable)method
                   callback:(const pybind::object &)cb;

- (void)activateSemaphore:(NSString * _Nonnull)name withValue:(id _Nullable)value;
- (Mengine::AppleSemaphoreListenerInterfacePtr)waitSemaphore:(NSString * _Nonnull)name
                                                    listener:(const Mengine::AppleSemaphoreListenerInterfacePtr &)listener;

@end
