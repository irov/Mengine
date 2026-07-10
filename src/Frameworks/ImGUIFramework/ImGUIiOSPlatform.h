#pragma once

#import <UIKit/UIKit.h>

@interface MengineImGUIiOSPlatform : NSObject

+ (instancetype)sharedInstance;

+ (BOOL)initializeWithView:(UIView *)_view;
+ (void)finalizeBackend;
+ (BOOL)isInitialized;

+ (void)newFrameWithView:(UIView *)_view;
+ (BOOL)handleTouches:(NSSet<UITouch *> *)_touches view:(UIView *)_view phase:(UITouchPhase)_phase;

@end
