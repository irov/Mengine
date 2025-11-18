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

- (void)completedConsent {
    self.m_completed = YES;
    
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
            
            [self completedConsent];
            
            return;
        }
        
        UMPPrivacyOptionsRequirementStatus privacyOptionsRequirementStatus = UMPConsentInformation.sharedInstance.privacyOptionsRequirementStatus;
        
        if (privacyOptionsRequirementStatus == UMPPrivacyOptionsRequirementStatusNotRequired) {
            IOS_LOGGER_MESSAGE(@"[UMP] privacyOptionsRequirementStatus not required");
            
            [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyOther];
            
            [self completedConsent];
            
            return;
        }
        
        [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyGDPR];
        
        UMPFormStatus formStatus = UMPConsentInformation.sharedInstance.formStatus;
        
        if (formStatus != UMPFormStatusAvailable) {
            IOS_LOGGER_MESSAGE(@"[UMP] formStatus not available: %ld", (long)formStatus);
            
            [self completedConsent];
            
            return;
        }
        
        UIViewController * rootVC = [iOSDetail getRootViewController];
            
        [UMPConsentForm loadAndPresentIfRequiredFromViewController:rootVC completionHandler:^(NSError * _Nullable loadError) {
            if (loadError != nil) {
                IOS_LOGGER_MESSAGE(@"[UMP] loadAndPresentIfRequiredFromViewController error: %@", loadError);
                
                [self completedConsent];

                return;
            }
            
            IOS_LOGGER_MESSAGE(@"[UMP] loadAndPresentIfRequiredFromViewController completed");
     
            // After form dismissal, UMP writes IABTCF_* to NSUserDefaults. Broadcast updated consent.
            [self completedConsent];
        }];
    }];
    
    return YES;
}

- (BOOL)isComplete {
    return self.m_completed
}

@end
