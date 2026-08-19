#pragma once

#import <UIKit/UIKit.h>

typedef void (^iOSSafeAreaInsetsDidChangeCallback)(void);

@protocol iOSSafeAreaProviderInterface <NSObject>

- (BOOL)getSafeAreaViewport:(CGRect * _Nonnull)viewport;
- (void)setSafeAreaInsetsDidChangeCallback:(iOSSafeAreaInsetsDidChangeCallback _Nullable)callback;

@end
