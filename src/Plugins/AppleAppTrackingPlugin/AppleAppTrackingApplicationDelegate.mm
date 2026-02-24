#import "AppleAppTrackingApplicationDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSAppTrackingTransparencyParam.h"

#include "Kernel/ScriptEmbeddingHelper.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleAppTrackingScriptEmbedding.h"
#endif

#import <AppTrackingTransparency/AppTrackingTransparency.h>
#import <AdSupport/AdSupport.h>

#define IDFA_NOT_DETERMINED @"00000000-0000-0000-0000-000000000000"

@implementation AppleAppTrackingApplicationDelegate

- (instancetype)init {
    self = [super init];
    
    self.m_status = EAATA_NOT_DETERMINED;
    self.m_idfa = IDFA_NOT_DETERMINED;
    
    return self;
}

- (void)updateTrackingStatus:(ATTrackingManagerAuthorizationStatus)status {    
    switch(status) {
        case ATTrackingManagerAuthorizationStatusAuthorized: {
            self.m_status = EAATA_AUTHORIZED;

            self.m_idfa = [AppleAppTrackingApplicationDelegate makeIDFA];
        }break;
        case ATTrackingManagerAuthorizationStatusDenied: {
            self.m_status = EAATA_DENIED;
            self.m_idfa = IDFA_NOT_DETERMINED;
        }break;
        case ATTrackingManagerAuthorizationStatusNotDetermined: {
            self.m_status = EAATA_NOT_DETERMINED;
            self.m_idfa = IDFA_NOT_DETERMINED;
        }break;
        case ATTrackingManagerAuthorizationStatusRestricted: {
            self.m_status = EAATA_RESTRICTED;
            self.m_idfa = IDFA_NOT_DETERMINED;
        }break;
    }
    
    iOSAppTrackingTransparencyParam * tracking = [[iOSAppTrackingTransparencyParam alloc] init];
    tracking.APPTRACKINGTRANSPARENCY_AUTHORIZATION = self.m_status;
    tracking.APPTRACKINGTRANSPARENCY_IDFA = self.m_idfa;
    
    [iOSDetail appTrackingTransparency:tracking];
}

#pragma mark - AppleAppTrackingInterface

+ (instancetype) sharedInstance {
    static AppleAppTrackingApplicationDelegate * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleAppTrackingApplicationDelegate class]];
    });
    return sharedInstance;
}

+ (NSString *)makeIDFA {
    NSUUID * idfa_uuid = [iOSDetail getAdIdentifier];

    if (idfa_uuid == nil) {
        return IDFA_NOT_DETERMINED;
    }

    NSString * idfa = [idfa_uuid UUIDString];
    
    return idfa;
}

- (void)authorization {
    IOS_LOGGER_MESSAGE( @"request app tracking authorization" );
    
    ATTrackingManagerAuthorizationStatus currentStatus = [ATTrackingManager trackingAuthorizationStatus];
    
    if (currentStatus != ATTrackingManagerAuthorizationStatusNotDetermined) {
        [self updateTrackingStatus:currentStatus];
        
        return;
    }
    
    __weak AppleAppTrackingApplicationDelegate * weakSelf = self;
    
    [iOSDetail addDidBecomeActiveOperationWithCompletion:^(void (^ _Nonnull completion)(void)) {
        AppleAppTrackingApplicationDelegate * strongSelf = weakSelf;
        
        if (strongSelf == nil) {
            completion();
            return;
        }
        
        ATTrackingManagerAuthorizationStatus status = [ATTrackingManager trackingAuthorizationStatus];
        
        if (status != ATTrackingManagerAuthorizationStatusNotDetermined) {
            [strongSelf updateTrackingStatus:status];
            completion();
            return;
        }
        
        [strongSelf requestAuthorizationWithCompletion:completion];
    }];
}

- (void)requestAuthorizationWithCompletion:(void (^ _Nonnull)(void))completion {
    IOS_LOGGER_MESSAGE( @"requesting ATT authorization dialog" );
    
    [ATTrackingManager requestTrackingAuthorizationWithCompletionHandler:^(ATTrackingManagerAuthorizationStatus status) {
        IOS_LOGGER_MESSAGE( @"app tracking authorization status: %lu", status );

        [self updateTrackingStatus:status];
        
        completion();
    }];
}

- (EAppleAppTrackingAuthorization)getAuthorizationStatus {
    return self.m_status;
}

- (NSString *)getIDFA {
    return self.m_idfa;
}

- (BOOL)isTrackingAllowed {
    switch( ATTrackingManager.trackingAuthorizationStatus ) {
        case ATTrackingManagerAuthorizationStatusAuthorized: {
            return YES;
        }break;
        case ATTrackingManagerAuthorizationStatusDenied: {
            return NO;
        }break;
        case ATTrackingManagerAuthorizationStatusNotDetermined: {
            return NO;
        }break;
        case ATTrackingManagerAuthorizationStatusRestricted: {
            return NO;
        }break;
    }
    
    return NO;
}

#pragma mark - iOSPluginTransparencyConsentDelegateInterface

- (void)onTransparencyConsent:(iOSTransparencyConsentParam *)consent {
    IOS_LOGGER_MESSAGE(@"UMP consent received, checking if ATT authorization should be requested");
    
    // Check if consent is still pending - don't request ATT if pending
    if ([consent isPending] == YES) {
        IOS_LOGGER_MESSAGE(@"Consent is pending, skipping ATT authorization request");
        return;
    }
    
    // Check if user granted consent for ad storage - if NO, user denied tracking in GDPR prompt
    // According to Apple guidelines, we should NOT request ATT if user denied tracking in GDPR
    BOOL adStorageConsent = [consent getConsentAdStorage];
    if (adStorageConsent == NO) {
        IOS_LOGGER_MESSAGE(@"User denied ad storage consent in GDPR prompt, skipping ATT authorization request");
        
        // Update tracking status based on current system status (but don't request ATT dialog)
        ATTrackingManagerAuthorizationStatus status = [ATTrackingManager trackingAuthorizationStatus];
        [self updateTrackingStatus:status];
        
        return;
    }
    
    // User granted consent for ad storage, now check ATT status
    ATTrackingManagerAuthorizationStatus status = [ATTrackingManager trackingAuthorizationStatus];
    
    if (status != ATTrackingManagerAuthorizationStatusNotDetermined) {
        [self updateTrackingStatus:status];
        return;
    }
    
    // User granted GDPR consent and ATT is not determined, request ATT authorization
    IOS_LOGGER_MESSAGE(@"User granted GDPR consent for ad storage, requesting ATT authorization");
    [self authorization];
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (void)onRunBegin {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::AppleAppTrackingScriptEmbedding>( MENGINE_DOCUMENT_FUNCTION );
#endif
}

- (void)onStopEnd {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::AppleAppTrackingScriptEmbedding>();
#endif
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    return YES;
}

@end
