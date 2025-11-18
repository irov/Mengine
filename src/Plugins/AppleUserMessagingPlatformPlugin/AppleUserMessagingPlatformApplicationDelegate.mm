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
    
    [iOSDetail transparencyConsent:consent];
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    UMPRequestParameters *parameters = [[UMPRequestParameters alloc] init];
    parameters.tagForUnderAgeOfConsent = NO;
    
    // For testing purposes, you can force a UMPDebugGeography of EEA or not EEA.
#if defined(MENGINE_DEBUG)
    UMPDebugSettings *debugSettings = [[UMPDebugSettings alloc] init];
    debugSettings.geography = UMPDebugGeographyEEA;
    debugSettings.testDeviceIdentifiers = @[@"46D5A6B4-FAE4-4EA3-9D36-94F9A7CEA291"];
    parameters.debugSettings = debugSettings;
#endif
    
    [UMPConsentInformation.sharedInstance requestConsentInfoUpdateWithParameters:parameters completionHandler:^(NSError * _Nullable error) {
        if (error != nil) {
            IOS_LOGGER_MESSAGE(@"[UMP] requestConsentInfoUpdate error: %@", error);
            
            return;
        }
        
        UMPPrivacyOptionsRequirementStatus privacyOptionsRequirementStatus = UMPConsentInformation.sharedInstance.privacyOptionsRequirementStatus;
        
        if (privacyOptionsRequirementStatus == UMPPrivacyOptionsRequirementStatusNotRequired) {
            IOS_LOGGER_MESSAGE(@"[UMP] privacyOptionsRequirementStatus not required");
            
            [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyOther];
            
            return;
        }
        
        [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyGDPR];
        
        UMPFormStatus formStatus = UMPConsentInformation.sharedInstance.formStatus;
        
        if (formStatus != UMPFormStatusAvailable) {
            IOS_LOGGER_MESSAGE(@"[UMP] formStatus not available: %ld", (long)formStatus);
            
            return;
        }
        
        UIViewController * rootVC = [iOSDetail getRootViewController];
            
        [UMPConsentForm loadAndPresentIfRequiredFromViewController:rootVC completionHandler:^(NSError * _Nullable loadError) {
            if (loadError != nil) {
                IOS_LOGGER_MESSAGE(@"[UMP] loadAndPresentIfRequiredFromViewController error: %@", loadError);

                return;
            }
     
            // After form dismissal, UMP writes IABTCF_* to NSUserDefaults. Broadcast updated consent.
            [self notifyConsentChangedFromDefaults];
        }];
    }];
    
    return YES;
}

@end
