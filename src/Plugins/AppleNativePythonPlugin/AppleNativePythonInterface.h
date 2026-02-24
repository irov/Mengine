#pragma once

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/Apple/AppleSemaphoreListenerInterface.h"
#include "Environment/Python/PythonIncluder.h"

@protocol AppleNativePythonInterface <NSObject>

+ (instancetype)sharedInstance;

- (pybind::object)addAppleCallback:(NSString *)plugin
                             method:(NSString *)method
                           callback:(const pybind::object &)cb
                               args:(const pybind::args &)args;
- (void)removeAppleCallback:(NSString *)plugin
                     method:(NSString *)method
                   callback:(const pybind::object &)cb;

- (void)activateSemaphore:(NSString *)name;
- (Mengine::AppleSemaphoreListenerInterfacePtr)waitSemaphore:(NSString *)name
                                                    listener:(const Mengine::AppleSemaphoreListenerInterfacePtr &)listener;

@end
