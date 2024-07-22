#import "AppleApplicationDelegates.h"

@implementation AppleApplicationDelegates

+ (NSArray *)getApplicationDelegates {
    NSArray * proxysClassed = [[NSArray alloc] initWithObjects:@"AppleGeneralDataProtectionRegulationApplicationDelegate", @"AppleAppLovinApplicationDelegate", @"AppleStoreInAppPurchaseApplicationDelegate", @"AppleFirebaseApplicationDelegate", @"AppleFirebaseAnalyticsApplicationDelegate", @"AppleFirebaseCrashlyticsApplicationDelegate",  nil];
    
    return proxysClassed;
}

@end
