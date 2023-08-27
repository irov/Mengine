#import "Environment/iOS/UIPluginApplicationDelegateInterface.h"

#import <Adjust/Adjust.h>

@interface AppleAdjustApplicationDelegate : NSObject<UIPluginApplicationDelegateInterface, AdjustDelegate>

@end
