#import "Environment/iOS/UIPluginApplicationDelegateInterface.h"

@interface AppleSentryApplicationDelegate : NSObject<UIPluginApplicationDelegateInterface>

@property (assign) BOOL m_sendAllow;

@end
