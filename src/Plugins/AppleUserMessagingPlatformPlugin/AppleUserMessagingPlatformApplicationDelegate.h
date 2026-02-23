#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

@interface AppleUserMessagingPlatformApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

+ (instancetype)sharedInstance;

- (void)showConsentFlow;
- (BOOL)isConsentFlowUserGeographyGDPR;

@property (atomic, assign) BOOL m_completed;

@end


