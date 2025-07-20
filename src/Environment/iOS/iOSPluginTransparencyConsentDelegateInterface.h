#pragma once

#include "Config/Config.h"

#import "Environment/iOS/iOSTransparencyConsentParam.h"

@protocol iOSPluginTransparencyConsentDelegateInterface <NSObject>

- (void)onTransparencyConsent:(iOSTransparencyConsentParam *)consent;

@end
