#import "Environment/iOS/UIProxyApplicationDelegateInterface.h"

@interface AppleSentryApplicationDelegate : NSObject<UIProxyApplicationDelegateInterface>

@property (assign) BOOL m_sendAllow;

@end
