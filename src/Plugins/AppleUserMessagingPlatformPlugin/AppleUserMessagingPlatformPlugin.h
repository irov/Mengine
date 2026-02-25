#import "Environment/iOS/iOSPluginInterface.h"

@interface AppleUserMessagingPlatformPlugin : NSObject<iOSPluginInterface>

+ (instancetype)sharedInstance;

- (void)showConsentFlow;
- (BOOL)isConsentFlowUserGeographyGDPR;

@property (atomic, assign) BOOL m_completed;

@end


