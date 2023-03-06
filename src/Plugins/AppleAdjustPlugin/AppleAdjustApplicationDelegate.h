#import "Environment/iOS/UIKitProxyApplicationDelegateInterface.h"

#import <Adjust/Adjust.h>

@interface AppleAdjustApplicationDelegate : NSObject<UIKitProxyApplicationDelegateInterface, AdjustDelegate>

@end
