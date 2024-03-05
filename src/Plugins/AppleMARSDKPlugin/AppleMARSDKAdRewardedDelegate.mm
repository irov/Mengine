#import "AppleMARSDKAdRewardedDelegate.h"

#include "Environment/Apple/AppleErrorHelper.h"
#include "Environment/iOS/iOSDetail.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadHelper.h"

@implementation AppleMARSDKAdRewardedDelegate

#pragma mark -

- (instancetype _Nonnull)initWithService:(Mengine::AppleMARSDKServiceInterface* _Nonnull)service {
    self = [super init];
    
    self.m_service = service;
    
    return self;
}

#pragma mark - MARAdRewardedDelegate

- (void) MARAdRewardedDidFailed:(MARAdErrorCode)code withMessage: (NSString *)message adDict:(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidFailed code: %d message: %s adDict: %s"
        , (int32_t)code
        , [message UTF8String]
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    Mengine::AppleMARSDKProviderInterfacePtr provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }
    
    Mengine::Helper::dispatchMainThreadEvent([provider]() {
        provider->onAdRewardedDidFailed();
    });
}

- (void) MARAdRewardedDidLoaded:(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidLoaded adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    Mengine::AppleMARSDKProviderInterfacePtr provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }
    
    Mengine::Helper::dispatchMainThreadEvent([provider]() {
        provider->onAdRewardedDidLoaded();
    });
}

- (void) MARAdRewardedDidShow:(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidShow adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    Mengine::AppleMARSDKProviderInterfacePtr provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }
    
    Mengine::Helper::dispatchMainThreadEvent([provider]() {
        provider->onAdRewardedDidShow();
    });
}

- (void) MARAdRewardedDidShowFailed:(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidShowFailed adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    Mengine::AppleMARSDKProviderInterfacePtr provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }
    
    Mengine::Helper::dispatchMainThreadEvent([provider]() {
        provider->onAdRewardedDidShowFailed();
    });
}

- (void) MARAdRewardedDidClicked:(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidClicked adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    Mengine::AppleMARSDKProviderInterfacePtr provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }
    
    Mengine::Helper::dispatchMainThreadEvent([provider]() {
        provider->onAdRewardedDidClicked();
    });
}

- (void) MARAdRewardedDidClosed:(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidClosed adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    Mengine::AppleMARSDKProviderInterfacePtr provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }
    
    Mengine::Helper::dispatchMainThreadEvent([provider]() {
        provider->onAdRewardedDidClosed();
    });
}

- (void) MARAdRewardedDidSkipped:(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidSkipped adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    Mengine::AppleMARSDKProviderInterfacePtr provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }
    
    Mengine::Helper::dispatchMainThreadEvent([provider]() {
        provider->onAdRewardedDidSkipped();
    });
}

- (void) MARAdRewardedDidFinished:(NSString *)itemName itemNum:(int)itemNum adDict:(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidFinished adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    Mengine::AppleMARSDKProviderInterfacePtr provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }
    
    Mengine::Helper::dispatchMainThreadEvent([provider, itemName, itemNum]() {
        const Mengine::Char * itemName_str = [itemName UTF8String];
        
        provider->onAdRewardedDidFinished( itemName_str, itemNum );
    });
}

@end
