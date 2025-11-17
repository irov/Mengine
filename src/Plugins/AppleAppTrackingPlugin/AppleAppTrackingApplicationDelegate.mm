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

- (void)makeIDFA {
    NSUUID * idfa_uuid = [iOSDetail getAdIdentifier];

    if (idfa_uuid == nil) {
        return;
    }

    NSString * idfa = [idfa_uuid UUIDString];
    
    self.m_idfa = idfa;
}

- (void)authorization:(void (^)(EAppleAppTrackingAuthorization status, NSString *idfa))response {
    if (@available(iOS 14.5, *)) {
        IOS_LOGGER_MESSAGE( @"request app tracking authorization" );
        
        [ATTrackingManager requestTrackingAuthorizationWithCompletionHandler:^(ATTrackingManagerAuthorizationStatus status) {
            switch( status ) {
                case ATTrackingManagerAuthorizationStatusAuthorized: {
                    self.m_status = EAATA_AUTHORIZED;
                    
                    [self makeIDFA];
                }break;
                case ATTrackingManagerAuthorizationStatusDenied: {
                    self.m_status = EAATA_DENIED;
                }break;
                case ATTrackingManagerAuthorizationStatusNotDetermined: {
                    self.m_status = EAATA_NOT_DETERMINED;
                }break;
                case ATTrackingManagerAuthorizationStatusRestricted: {
                    self.m_status = EAATA_RESTRICTED;
                }break;
            }
            
            IOS_LOGGER_MESSAGE( @"app tracking authorization status: %lu", self.m_status );
            
            [AppleDetail addMainQueueOperation:^{
                response( self.m_status, self.m_idfa );
            }];
        }];
        
        return;
    }
    
    self.m_status = EAATA_AUTHORIZED;

    [self makeIDFA];
    
    IOS_LOGGER_MESSAGE( @"app tracking authorization status: %lu", self.m_status );

    response( self.m_status, self.m_idfa );
}

- (EAppleAppTrackingAuthorization)getAuthorizationStatus {
    return self.m_status;
}

- (NSString *)getIDFA {
    return self.m_idfa;
}

- (BOOL)isTrackingAllowed {
    if (@available(iOS 14.5, *)) {
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

    if ([iOSDetail isValidIDFA] == NO) {
        return NO;
    }
    
    return YES;
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    return YES;
}

@end
