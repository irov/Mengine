#import "Environment/iOS/UIKitProxyApplicationDelegateBase.h"

#import <Adjust.h>

@interface AppleAdjustApplicationDelegate : NSObject<UIKitProxyApplicationDelegateBase, AdjustDelegate>

@end
