#import "AppleUserMessagingPlatformApplicationDelegate.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSTransparencyConsentParam.h"
#import "Environment/iOS/iOSConsentFlowUserGeography.h"

#import <UserMessagingPlatform/UserMessagingPlatform.h>

@implementation AppleUserMessagingPlatformApplicationDelegate

+ (instancetype)sharedInstance {
    static AppleUserMessagingPlatformApplicationDelegate * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleUserMessagingPlatformApplicationDelegate class]];
    });
    return sharedInstance;
}

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

- (void)showConsentFlow {
    UMPPrivacyOptionsRequirementStatus privacyOptionsRequirementStatus = UMPConsentInformation.sharedInstance.privacyOptionsRequirementStatus;

    if (privacyOptionsRequirementStatus == UMPPrivacyOptionsRequirementStatusNotRequired) {
        IOS_LOGGER_MESSAGE(@"[UMP] privacy options form not required");

        return;
    }

    [iOSDetail addDidBecomeActiveOperationWithCompletion:^(void (^ _Nonnull completion)(void)) {
        UIViewController * rootVC = [iOSDetail getRootViewController];

        [UMPConsentForm presentPrivacyOptionsFormFromViewController:rootVC completionHandler:^(NSError * _Nullable error) {
            if (error != nil) {
                IOS_LOGGER_MESSAGE(@"[UMP] presentPrivacyOptionsFormFromViewController error: %@", error);
            } else {
                IOS_LOGGER_MESSAGE(@"[UMP] presentPrivacyOptionsFormFromViewController completed");
            }

            // UMP writes IABTCF_* values to NSUserDefaults after dismissal.
            iOSTransparencyConsentParam * consent = [[iOSTransparencyConsentParam alloc] initFromUserDefaults];

            [iOSDetail transparencyConsent:consent];

            completion();
        }];
    }];
}

- (BOOL)isConsentFlowUserGeographyGDPR {
    iOSConsentFlowUserGeography userGeography = [iOSTransparencyConsentParam getConsentFlowUserGeography];

    return userGeography == iOSConsentFlowUserGeographyGDPR;
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    UMPRequestParameters * parameters = [[UMPRequestParameters alloc] init];
    parameters.tagForUnderAgeOfConsent = NO;
    
    // For testing purposes, you can force a UMPDebugGeography of EEA or not EEA.
#if defined(MENGINE_DEBUG)
    UMPDebugSettings *debugSettings = [[UMPDebugSettings alloc] init];
    debugSettings.geography = UMPDebugGeographyEEA;
    //debugSettings.testDeviceIdentifiers = @[@"DB1869F9-B9D0-4D84-B2CB-F63F249E58C9"];
    parameters.debugSettings = debugSettings;
#endif
    
    __weak AppleUserMessagingPlatformApplicationDelegate * weakSelf = self;
    
    [UMPConsentInformation.sharedInstance requestConsentInfoUpdateWithParameters:parameters completionHandler:^(NSError * _Nullable error) {
        AppleUserMessagingPlatformApplicationDelegate * strongSelf = weakSelf;
        
        if (strongSelf == nil) {
            return;
        }
        
        if (error != nil) {
            IOS_LOGGER_MESSAGE(@"[UMP] requestConsentInfoUpdate error: %@", error);
            
            [strongSelf completedConsent];
            
            return;
        }
        
        UMPPrivacyOptionsRequirementStatus privacyOptionsRequirementStatus = UMPConsentInformation.sharedInstance.privacyOptionsRequirementStatus;
        
        if (privacyOptionsRequirementStatus == UMPPrivacyOptionsRequirementStatusNotRequired) {
            IOS_LOGGER_MESSAGE(@"[UMP] privacyOptionsRequirementStatus not required");
            
            [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyOther];
            
            [strongSelf completedConsent];
            
            return;
        }
        
        [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyGDPR];
        
        UMPFormStatus formStatus = UMPConsentInformation.sharedInstance.formStatus;
        
        if (formStatus != UMPFormStatusAvailable) {
            IOS_LOGGER_MESSAGE(@"[UMP] formStatus not available: %ld", (long)formStatus);
            
            [strongSelf completedConsent];
            
            return;
        }
        
        [iOSDetail addDidBecomeActiveOperationWithCompletion:^(void (^ _Nonnull completion)(void)) {
            AppleUserMessagingPlatformApplicationDelegate * strongSelf2 = weakSelf;
            
            if (strongSelf2 == nil) {
                completion();
                return;
            }
            
            UIViewController * rootVC = [iOSDetail getRootViewController];
                
            [UMPConsentForm loadAndPresentIfRequiredFromViewController:rootVC completionHandler:^(NSError * _Nullable loadError) {
                if (loadError != nil) {
                    IOS_LOGGER_MESSAGE(@"[UMP] loadAndPresentIfRequiredFromViewController error: %@", loadError);
                    
                    [strongSelf2 completedConsent];
                    completion();

                    return;
                }
                
                IOS_LOGGER_MESSAGE(@"[UMP] loadAndPresentIfRequiredFromViewController completed");
         
                // After form dismissal, UMP writes IABTCF_* to NSUserDefaults. Broadcast updated consent.
                [strongSelf2 completedConsent];
                completion();
            }];
        }];
    }];
    
    return YES;
}

- (BOOL)isComplete {
    return self.m_completed;
}

@end
