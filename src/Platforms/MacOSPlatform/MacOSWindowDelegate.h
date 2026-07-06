#pragma once

#import "MacOSPlatformService.h"

#import <AppKit/AppKit.h>

@interface MacOSWindowDelegate : NSObject<NSWindowDelegate>

- (instancetype)initWithPlatformService:(Mengine::MacOSPlatformService *)service;

@end
