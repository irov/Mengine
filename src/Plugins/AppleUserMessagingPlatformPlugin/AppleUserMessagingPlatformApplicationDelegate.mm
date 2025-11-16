#import "AppleUserMessagingPlatformApplicationDelegate.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSTransparencyConsentParam.h"
#import "Environment/iOS/iOSConsentFlowUserGeography.h"

#import <UserMessagingPlatform/UserMessagingPlatform.h>

@implementation AppleUserMessagingPlatformApplicationDelegate

#pragma mark - helpers

- (void)notifyConsentChangedFromDefaults {
    iOSTransparencyConsentParam * consent = [[iOSTransparencyConsentParam alloc] initFromUserDefaults];
    
    // Infer geography based on IABTCF_gdprApplies when present
    if ([consent isEEA] == YES) {
        [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyGDPR];
    } else {
        [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyOther];
    }
    
    [iOSDetail transparencyConsent:consent];
}

- (void)runUMPFlow {
    UMPRequestParameters *parameters = [[UMPRequestParameters alloc] init];
    // Debug settings can be added later via bundle config if needed
    
    [UMPConsentInformation.sharedInstance requestConsentInfoUpdateWithParameters:parameters completionHandler:^(NSError * _Nullable error) {
        if (error != nil) {
            IOS_LOGGER_MESSAGE(@"[UMP] requestConsentInfoUpdate error: %@", error);
            [self notifyConsentChangedFromDefaults];
            return;
        }
        
        UMPFormStatus formStatus = UMPConsentInformation.sharedInstance.formStatus;
        if (formStatus == UMPFormStatusAvailable) {
            [UMPConsentForm loadWithCompletionHandler:^(UMPConsentForm * _Nullable form, NSError * _Nullable loadError) {
                if (loadError != nil) {
                    IOS_LOGGER_MESSAGE(@"[UMP] loadWithCompletionHandler error: %@", loadError);
                    [self notifyConsentChangedFromDefaults];
                    return;
                }
                
                UIViewController * rootVC = [iOSDetail getRootViewController];
                [form presentFromViewController:rootVC completionHandler:^(NSError * _Nullable dismissError) {
                    if (dismissError != nil) {
                        IOS_LOGGER_MESSAGE(@"[UMP] present dismiss error: %@", dismissError);
                    }
                    
                    // After form dismissal, UMP writes IABTCF_* to NSUserDefaults. Broadcast updated consent.
                    [self notifyConsentChangedFromDefaults];
                }];
            }];
        } else {
            // No form to show; just propagate current consent state from defaults
            [self notifyConsentChangedFromDefaults];
        }
    }];
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [self runUMPFlow];
    return YES;
}

@end


