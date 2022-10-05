#import "AppleMARSDKAdRewardedDelegate.h"

#include "Environment/Apple/AppleErrorHelper.h"
#include "Environment/iOS/iOSDetail.h"

#include "Kernel/Logger.h"

@implementation AppleMARSDKAdRewardedDelegate

#pragma mark -

@synthesize m_service;

- (instancetype _Nonnull)initWithService: (Mengine::AppleMARSDKServiceInterface* _Nonnull)service {
    self = [super init];
    
    self.m_service = service;
    
    return self;
}

- (void) MARAdRewardedDidFailed: (MARAdErrorCode)code withMessage: (NSString *)message adDict:(NSDictionary *)adDict {
    LOGGER_INFO( "marsdk", "MARAdRewardedDidFailed" );
}

- (void) MARAdRewardedDidLoaded :(NSDictionary *)adDict {
    LOGGER_INFO( "marsdk", "MARAdRewardedDidLoaded" );
}

- (void) MARAdRewardedDidShow :(NSDictionary *)adDict {
    LOGGER_INFO( "marsdk", "MARAdRewardedDidShow" );
}

- (void) MARAdRewardedDidClicked :(NSDictionary *)adDict {
    LOGGER_INFO( "marsdk", "MARAdRewardedDidClicked" );
}

- (void) MARAdRewardedDidClosed :(NSDictionary *)adDict {
    LOGGER_INFO( "marsdk", "MARAdRewardedDidClosed" );
}

- (void) MARAdRewardedDidSkipped :(NSDictionary *)adDict {
    LOGGER_INFO( "marsdk", "MARAdRewardedDidSkipped" );
}

- (void) MARAdRewardedDidFinished: (NSString *)itemName itemNum:(int)itemNum adDict:(NSDictionary *)adDict {
    LOGGER_INFO( "marsdk", "MARAdRewardedDidFinished" );
}

@end
