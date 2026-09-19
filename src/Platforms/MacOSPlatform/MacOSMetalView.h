#pragma once

#include "Config/Config.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)

#import <AppKit/AppKit.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

@interface MacOSMetalView : NSView

- (instancetype)initWithFrame:(NSRect)frameRect device:(id<MTLDevice>)device;

- (void)updateVSync:(BOOL)vsync;

- (NSInteger)drawableWidth;
- (NSInteger)drawableHeight;

- (id<MTLDevice>)metalDevice;

@end

#endif
