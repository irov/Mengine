#import "MengineAppleApplicationDelegates.h"

NSArray * getMengineAppleApplicationDelegates() {
    NSArray * proxysClassed = [[NSArray alloc] initWithObjects:@"AppleGeneralDataProtectionRegulationApplicationDelegate", @"AppleAppLovinApplicationDelegate", @"AppleStoreInAppPurchaseApplicationDelegate", @"AppleFirebaseApplicationDelegate", @"AppleFirebaseAnalyticsApplicationDelegate",  nil];
    
    return proxysClassed;
}
