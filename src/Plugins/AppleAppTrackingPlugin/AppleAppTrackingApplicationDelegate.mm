#import "AppleAppTrackingApplicationDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#import <AppTrackingTransparency/AppTrackingTransparency.h>
#import <AdSupport/AdSupport.h>

@implementation AppleAppTrackingApplicationDelegate

- (instancetype)init {
    self = [super init];
    
    self.m_status = EAATA_NOT_DETERMINED;
    self.m_idfa = @"00000000-0000-0000-0000-000000000000";
    
    return self;
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
        return @"00000000-0000-0000-0000-000000000000";
    }

    NSString * idfa = [idfa_uuid UUIDString];
    
    return idfa;
}

- (void)authorization {
    IOS_LOGGER_MESSAGE( @"request app tracking authorization" );
    
    [ATTrackingManager requestTrackingAuthorizationWithCompletionHandler:^(ATTrackingManagerAuthorizationStatus status) {
        switch( status ) {
            case ATTrackingManagerAuthorizationStatusAuthorized: {
                self.m_status = EAATA_AUTHORIZED;
                
                self.m_idfa = [AppleAppTrackingApplicationDelegate makeIDFA];
            }break;
            case ATTrackingManagerAuthorizationStatusDenied: {
                self.m_status = EAATA_DENIED;
                self.m_idfa = @"00000000-0000-0000-0000-000000000000";
            }break;
            case ATTrackingManagerAuthorizationStatusNotDetermined: {
                self.m_status = EAATA_NOT_DETERMINED;
                self.m_idfa = @"00000000-0000-0000-0000-000000000000";
            }break;
            case ATTrackingManagerAuthorizationStatusRestricted: {
                self.m_status = EAATA_RESTRICTED;
                self.m_idfa = @"00000000-0000-0000-0000-000000000000";
            }break;
        }
        
        IOS_LOGGER_MESSAGE( @"app tracking authorization status: %lu", self.m_status );
        
        iOSAppTrackingTransparencyParam * tracking = [[iOSAppTrackingTransparencyParam alloc] init];
        tracking.APPTRACKINGTRANSPARENCY_AUTHORIZATION = self.m_status;
        tracking.APPTRACKINGTRANSPARENCY_IDFA = self.m_idfa;
        
        [iOSDetail appTrackingTransparency:tracking];
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

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    return YES;
}

@end
