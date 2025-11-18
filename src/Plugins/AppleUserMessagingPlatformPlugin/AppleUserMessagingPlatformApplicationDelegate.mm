#import "AppleUserMessagingPlatformApplicationDelegate.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSTransparencyConsentParam.h"
#import "Environment/iOS/iOSConsentFlowUserGeography.h"

#import <UserMessagingPlatform/UserMessagingPlatform.h>

@implementation AppleUserMessagingPlatformApplicationDelegate

- (instancetype)init {
    self = [super init];
    
    if (self) {
        self.m_completed = NO;
    }
    
    return self;
}

#pragma mark - Details

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
    //debugSettings.testDeviceIdentifiers = @[@"DB1869F9-B9D0-4D84-B2CB-F63F249E58C9"];
    parameters.debugSettings = debugSettings;
#endif
    
    [UMPConsentInformation.sharedInstance requestConsentInfoUpdateWithParameters:parameters completionHandler:^(NSError * _Nullable error) {
        if (error != nil) {
            IOS_LOGGER_MESSAGE(@"[UMP] requestConsentInfoUpdate error: %@", error);
            
            self.m_completed = YES;
            
            return;
        }
        
        UMPPrivacyOptionsRequirementStatus privacyOptionsRequirementStatus = UMPConsentInformation.sharedInstance.privacyOptionsRequirementStatus;
        
        if (privacyOptionsRequirementStatus == UMPPrivacyOptionsRequirementStatusNotRequired) {
            IOS_LOGGER_MESSAGE(@"[UMP] privacyOptionsRequirementStatus not required");
            
            [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyOther];
            
            self.m_completed = YES;
            
            return;
        }
        
        [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyGDPR];
        
        UMPFormStatus formStatus = UMPConsentInformation.sharedInstance.formStatus;
        
        if (formStatus != UMPFormStatusAvailable) {
            IOS_LOGGER_MESSAGE(@"[UMP] formStatus not available: %ld", (long)formStatus);
            
            self.m_completed = YES;
            
            return;
        }
        
        UIViewController * rootVC = [iOSDetail getRootViewController];
            
        [UMPConsentForm loadAndPresentIfRequiredFromViewController:rootVC completionHandler:^(NSError * _Nullable loadError) {
            if (loadError != nil) {
                IOS_LOGGER_MESSAGE(@"[UMP] loadAndPresentIfRequiredFromViewController error: %@", loadError);
                
                self.m_completed = YES;

                return;
            }
     
            // After form dismissal, UMP writes IABTCF_* to NSUserDefaults. Broadcast updated consent.
            [self notifyConsentChangedFromDefaults];
        }];
    }];
    
    return YES;
}

@end
