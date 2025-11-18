#pragma once

#include "Config/Config.h"

#import "Environment/iOS/iOSAppTrackingTransparencyParam.h"

@protocol iOSPluginAppTrackingTransparencyDelegateInterface <NSObject>

- (void)onAppTrackingTransparency:(iOSAppTrackingTransparencyParam *)param;

@end
