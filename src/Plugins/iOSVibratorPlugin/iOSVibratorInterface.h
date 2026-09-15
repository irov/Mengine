#pragma once

#import <Foundation/Foundation.h>

@protocol iOSVibratorInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)prepare;
- (void)impact:(CGFloat)intensity sharpness:(CGFloat)sharpness;

- (void)mute:(BOOL)mute;
- (BOOL)isMute;

@end
