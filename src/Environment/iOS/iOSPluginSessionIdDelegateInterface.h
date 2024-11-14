#include "Config/Config.h"

#import "Environment/iOS/iOSSessionIdParam.h"

@protocol iOSPluginSessionIdDelegateInterface <NSObject>

- (void)onSessionId:(iOSSessionIdParam *)session;
- (void)onRemoveSessionData;

@end
