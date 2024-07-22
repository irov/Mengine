#import "AppleGeneralDataProtectionRegulationApplicationDelegate.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/Apple/AppleUserDefaults.h"

@implementation AppleGeneralDataProtectionRegulationApplicationDelegate

- (void)setGDPRPass:(BOOL)passGDPR {
    [AppleUserDefaults setBooleanForKey:@("mengine.gdpr.pass") value:passGDPR];
    
    [iOSDetail eventNotify:AppleEvent.EVENT_GDPR_PASS args:@[@(passGDPR)]];
}

- (BOOL)isGDPRPass {
    BOOL passGDPR = [AppleUserDefaults getBooleanForKey:@("mengine.gdpr.pass") defaultValue:NO];
    
    return passGDPR;
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    BOOL passGDPR = [AppleUserDefaults getBooleanForKey:@("mengine.gdpr.pass") defaultValue:NO];
    
    [iOSDetail eventNotify:AppleEvent.EVENT_GDPR_PASS args:@[@(passGDPR)]];
    
    return YES;
}

@end
