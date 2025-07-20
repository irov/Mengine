#pragma once

#include "Config/Config.h"

#import "Environment/iOS/iOSUserParam.h"

@protocol iOSPluginUserIdDelegateInterface <NSObject>

- (void)onUserId:(iOSUserParam *)user;
- (void)onRemoveUserData;

@end
