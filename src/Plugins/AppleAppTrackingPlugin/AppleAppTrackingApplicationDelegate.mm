#import "AppleAppTrackingApplicationDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSAppTrackingTransparencyParam.h"

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
    
    [ATTrackingManager requestTrackingAuthorizationWithCompletionHandler:^(ATTrackingManagerAuthorizationStatus status) {
        IOS_LOGGER_MESSAGE( @"app tracking authorization status: %lu", status );

        [self updateTrackingStatus:status];
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
    IOS_LOGGER_MESSAGE(@"UMP consent received, requesting ATT authorization");
    
    ATTrackingManagerAuthorizationStatus status = [ATTrackingManager trackingAuthorizationStatus];
    
    if (status != ATTrackingManagerAuthorizationStatusNotDetermined) {
        [self updateTrackingStatus:status];

        return;
    }
    
    [self authorization];
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    return YES;
}

@end
