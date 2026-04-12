#pragma once

#include "Config/Config.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)

#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

@interface iOSMetalView : UIView

- (instancetype)initWithFrame:(CGRect)frame device:(id<MTLDevice>)device;

- (BOOL)beginRender;
- (BOOL)endRender;

- (NSInteger)drawableWidth;
- (NSInteger)drawableHeight;

- (id<MTLDevice>)metalDevice;
- (id<CAMetalDrawable>)currentDrawable;

@end

#endif
