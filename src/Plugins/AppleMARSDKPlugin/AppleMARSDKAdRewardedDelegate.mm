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
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidFailed();
}

- (void) MARAdRewardedDidLoaded :(NSDictionary *)adDict {
    LOGGER_INFO( "marsdk", "MARAdRewardedDidLoaded" );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidLoaded();
}

- (void) MARAdRewardedDidShow :(NSDictionary *)adDict {
    LOGGER_INFO( "marsdk", "MARAdRewardedDidShow" );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidShow();
}

- (void) MARAdRewardedDidClicked :(NSDictionary *)adDict {
    LOGGER_INFO( "marsdk", "MARAdRewardedDidClicked" );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidClicked();
}

- (void) MARAdRewardedDidClosed :(NSDictionary *)adDict {
    LOGGER_INFO( "marsdk", "MARAdRewardedDidClosed" );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidClosed();
}

- (void) MARAdRewardedDidSkipped :(NSDictionary *)adDict {
    LOGGER_INFO( "marsdk", "MARAdRewardedDidSkipped" );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidSkipped();
}

- (void) MARAdRewardedDidFinished: (NSString *)itemName itemNum:(int)itemNum adDict:(NSDictionary *)adDict {
    LOGGER_INFO( "marsdk", "MARAdRewardedDidFinished" );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidFinished([itemName UTF8String], itemNum);
}

@end
