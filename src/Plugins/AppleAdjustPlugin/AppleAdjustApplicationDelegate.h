#import "Environment/iOS/UIProxyApplicationDelegateInterface.h"

#import <Adjust/Adjust.h>

@interface AppleAdjustApplicationDelegate : NSObject<UIProxyApplicationDelegateInterface, AdjustDelegate>

@end
