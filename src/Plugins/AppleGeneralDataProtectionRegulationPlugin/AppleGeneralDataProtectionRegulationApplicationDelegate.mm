#import "AppleGeneralDataProtectionRegulationApplicationDelegate.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/Apple/AppleUserDefaults.h"

@implementation AppleGeneralDataProtectionRegulationApplicationDelegate

- (void)setGDPRPass:(BOOL)passGDPR {
    Mengine::Helper::AppleSetUserDefaultsBoolean( @("mengine.gdpr.pass"), passGDPR );
    
    Mengine::Helper::iOSEventNotify( AppleEvent.EVENT_GDPR_PASS, @(passGDPR), nil );
}

- (BOOL)isGDPRPass {
    bool passGDPR = Mengine::Helper::AppleGetUserDefaultsBoolean( @("mengine.gdpr.pass"), false );
    
    return passGDPR;
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    bool passGDPR = Mengine::Helper::AppleGetUserDefaultsBoolean( @("mengine.gdpr.pass"), false );
    
    Mengine::Helper::iOSEventNotify( AppleEvent.EVENT_GDPR_PASS, @(passGDPR), nil );
    
    return YES;
}

@end
