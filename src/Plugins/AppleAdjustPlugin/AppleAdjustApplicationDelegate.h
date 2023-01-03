#import "Interface/UIKitProxyApplicationDelegateInterface.h"

#import <Foundation/Foundation.h>
#import <Adjust.h>

@interface AppleAdjustApplicationDelegate : NSObject<UIKitProxyApplicationDelegateInterface, AdjustDelegate>

@end
