#import "iOSUserMessagingPlatformPlugin.h"

#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSTransparencyConsentParam.h"
#import "Environment/iOS/iOSConsentFlowUserGeography.h"

#include "Kernel/ScriptEmbeddingHelper.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "iOSUserMessagingPlatformScriptEmbedding.h"
#endif

#import <UserMessagingPlatform/UserMessagingPlatform.h>

@implementation iOSUserMessagingPlatformPlugin

+ (instancetype)sharedInstance {
    static iOSUserMessagingPlatformPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[iOSUserMessagingPlatformPlugin class]];
    });
    return sharedInstance;
}

- (instancetype)init {
    self = [super init];

    if (self) {
        self.m_completed = NO;
        self.m_consentState = iOSUserMessagingPlatformConsentStatePending;
        self.m_completionHandlers = [NSMutableArray array];
    }

    return self;
}

#pragma mark - Details

- (void)completedConsent:(BOOL)successful {
    self.m_completed = YES;
    self.m_consentState = successful == YES ? iOSUserMessagingPlatformConsentStateCompleted : iOSUserMessagingPlatformConsentStateFailed;

    if (successful == YES) {
        iOSTransparencyConsentParam * consent = [[iOSTransparencyConsentParam alloc] initFromUserDefaults];
        consent.TRANSPARENCYCONSENT_CANREQUESTADS = UMPConsentInformation.sharedInstance.canRequestAds;

        [iOSDetail transparencyConsent:consent];
    }

    NSArray<void (^)(void)> * completionHandlers = [self.m_completionHandlers copy];
    [self.m_completionHandlers removeAllObjects];

    [AppleDetail addMainQueueOperation:^{
        for (void (^ completion)(void) in completionHandlers) {
            completion();
        }
    }];
}

- (void)waitForConsentCompletion:(void (^ _Nonnull)(void))completion {
    if (self.m_completed == YES) {
        [AppleDetail addMainQueueOperation:completion];
        return;
    }

    [self.m_completionHandlers addObject:[completion copy]];
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
            consent.TRANSPARENCYCONSENT_CANREQUESTADS = UMPConsentInformation.sharedInstance.canRequestAds;

            [iOSDetail transparencyConsent:consent];

            completion();
        }];
    }];
}

- (BOOL)isConsentFlowUserGeographyGDPR {
    iOSConsentFlowUserGeography userGeography = [iOSTransparencyConsentParam getConsentFlowUserGeography];

    return userGeography == iOSConsentFlowUserGeographyGDPR;
}

#pragma mark - iOSPluginInterface

- (void)onRunBegin {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::iOSUserMessagingPlatformScriptEmbedding>( MENGINE_DOCUMENT_FUNCTION );
#endif
}

- (void)onStopEnd {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::iOSUserMessagingPlatformScriptEmbedding>();
#endif
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [self requestConsentInfoUpdate];

    return YES;
}

- (void)requestConsentInfoUpdate {
    self.m_completed = NO;
    self.m_consentState = iOSUserMessagingPlatformConsentStatePending;

    UMPRequestParameters * parameters = [[UMPRequestParameters alloc] init];
    parameters.tagForUnderAgeOfConsent = NO;

    // For testing purposes, you can force a UMPDebugGeography of EEA or not EEA.
#if defined(MENGINE_DEBUG)
    UMPDebugSettings *debugSettings = [[UMPDebugSettings alloc] init];
    debugSettings.geography = UMPDebugGeographyEEA;
    //debugSettings.testDeviceIdentifiers = @[@"DB1869F9-B9D0-4D84-B2CB-F63F249E58C9"];
    parameters.debugSettings = debugSettings;
#endif

    __weak iOSUserMessagingPlatformPlugin * weakSelf = self;

    [UMPConsentInformation.sharedInstance requestConsentInfoUpdateWithParameters:parameters completionHandler:^(NSError * _Nullable error) {
        iOSUserMessagingPlatformPlugin * strongSelf = weakSelf;

        if (strongSelf == nil) {
            return;
        }

        if (error != nil) {
            IOS_LOGGER_MESSAGE(@"[UMP] requestConsentInfoUpdate error: %@", error);

            [strongSelf completedConsent:NO];

            return;
        }

        UMPPrivacyOptionsRequirementStatus privacyOptionsRequirementStatus = UMPConsentInformation.sharedInstance.privacyOptionsRequirementStatus;

        if (privacyOptionsRequirementStatus == UMPPrivacyOptionsRequirementStatusNotRequired) {
            IOS_LOGGER_MESSAGE(@"[UMP] privacyOptionsRequirementStatus not required");

            [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyOther];

        } else {
            [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyGDPR];
        }

        [iOSDetail addDidBecomeActiveOperationWithCompletion:^(void (^ _Nonnull completion)(void)) {
            iOSUserMessagingPlatformPlugin * strongSelf2 = weakSelf;

            if (strongSelf2 == nil) {
                completion();
                return;
            }

            UIViewController * rootVC = [iOSDetail getRootViewController];

            [UMPConsentForm loadAndPresentIfRequiredFromViewController:rootVC completionHandler:^(NSError * _Nullable loadError) {
                if (loadError != nil) {
                    IOS_LOGGER_MESSAGE(@"[UMP] loadAndPresentIfRequiredFromViewController error: %@", loadError);

                    [strongSelf2 completedConsent:NO];
                    completion();

                    return;
                }

                IOS_LOGGER_MESSAGE(@"[UMP] loadAndPresentIfRequiredFromViewController completed");

                // After form dismissal, UMP writes IABTCF_* to NSUserDefaults. Broadcast updated consent.
                [strongSelf2 completedConsent:YES];
                completion();
            }];
        }];
    }];

}

- (BOOL)isComplete {
    return self.m_completed;
}

- (iOSUserMessagingPlatformConsentState)getConsentState {
    return self.m_consentState;
}

- (void)retryConsent {
    if (self.m_consentState != iOSUserMessagingPlatformConsentStateFailed) {
        return;
    }

    self.m_completed = NO;
    self.m_consentState = iOSUserMessagingPlatformConsentStatePending;

    [AppleDetail addMainQueueOperation:^{
        [self requestConsentInfoUpdate];
    }];
}

@end
