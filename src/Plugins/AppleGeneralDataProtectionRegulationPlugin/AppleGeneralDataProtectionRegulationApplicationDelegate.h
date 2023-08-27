#import "Environment/iOS/UIProxyApplicationDelegateInterface.h"

@interface AppleGeneralDataProtectionRegulationApplicationDelegate : NSObject<UIProxyApplicationDelegateInterface>

- (void) setGDPRPass:(BOOL) passGDPR;
- (BOOL) isGDPRPass;

@end
