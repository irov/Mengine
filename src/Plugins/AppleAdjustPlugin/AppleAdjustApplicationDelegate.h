#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import <Adjust/Adjust.h>

@interface AppleAdjustApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AdjustDelegate>

@end
