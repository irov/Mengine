#pragma once

#import "Environment/iOS/iOSSafeAreaProviderInterface.h"

#import <UIKit/UIKit.h>

@interface iOSViewController : UIViewController<iOSSafeAreaProviderInterface>

- (void)showLaunchScreen;
- (BOOL)dismissLaunchScreen;

@end
