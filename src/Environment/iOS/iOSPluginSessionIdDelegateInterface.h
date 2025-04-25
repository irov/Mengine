#include "Config/Config.h"

#import "Environment/iOS/iOSUserParam.h"

@protocol iOSPluginSessionIdDelegateInterface <NSObject>

- (void)onSessionId:(iOSUserParam *)session;
- (void)onRemoveSessionData;

@end
