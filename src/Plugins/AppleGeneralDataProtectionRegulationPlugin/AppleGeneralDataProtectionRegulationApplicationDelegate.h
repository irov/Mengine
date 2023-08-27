#import "Environment/iOS/UIPluginApplicationDelegateInterface.h"

@interface AppleGeneralDataProtectionRegulationApplicationDelegate : NSObject<UIPluginApplicationDelegateInterface>

- (void) setGDPRPass:(BOOL) passGDPR;
- (BOOL) isGDPRPass;

@end
