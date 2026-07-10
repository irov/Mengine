#pragma once

#import <Foundation/Foundation.h>

@class NSEvent;
@class NSView;

@interface MengineImGUIMacOSPlatform : NSObject

+ (instancetype)sharedInstance;

+ (BOOL)initializeWithView:(NSView *)_view;
+ (void)finalizeBackend;
+ (BOOL)isInitialized;

+ (void)newFrameWithView:(NSView *)_view;
+ (BOOL)handleEvent:(NSEvent *)_event view:(NSView *)_view;

@end
