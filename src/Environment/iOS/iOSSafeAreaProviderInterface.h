#pragma once

#import <UIKit/UIKit.h>

typedef void (^iOSSafeAreaViewportChangedCallback)(void);

@protocol iOSSafeAreaProviderInterface <NSObject>

- (BOOL)getSafeAreaViewport:(CGRect * _Nonnull)viewport;
- (void)setSafeAreaViewportChangedCallback:(iOSSafeAreaViewportChangedCallback _Nullable)callback;

@end
