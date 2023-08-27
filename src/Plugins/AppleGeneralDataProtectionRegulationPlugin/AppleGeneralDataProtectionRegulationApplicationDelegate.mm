#import "AppleGeneralDataProtectionRegulationApplicationDelegate.h"

#include "Environment/Apple/AppleUserDefaults.h"

@implementation AppleGeneralDataProtectionRegulationApplicationDelegate

#pragma mark - UIProxyApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    bool passGDPR = Mengine::Helper::AppleGetUserDefaultsBoolean( "mengine.gdpr.pass", false );
    
    
    return YES;
}

@end
