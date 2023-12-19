#import "AppleMARSDKDelegate.h"

#include "Environment/Apple/AppleErrorHelper.h"
#include "Environment/iOS/iOSDetail.h"

#include "Kernel/Logger.h"

@implementation AppleMARSDKDelegate

#pragma mark -

- (instancetype _Nonnull)initWithService: (Mengine::AppleMARSDKServiceInterface* _Nonnull)service {
    self = [super init];
    
    self.m_service = service;
    
    return self;
}

- (UIView *)GetView {
    UIWindow * appWindow = [UIApplication sharedApplication].delegate.window;
    
    UIView * view = [appWindow.subviews objectAtIndex:0];
    
    return view;
}

- (UIViewController *)GetViewController {
    UIViewController * rootViewController = Mengine::Helper::iOSGetRootViewController();
    
    return rootViewController;
}

- (void) OnPlatformInit: (NSDictionary *)params {
    LOGGER_MESSAGE( "OnPlatformInit params: %s"
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }

    Mengine::MARSDKResultParams result_params;
    Mengine::Helper::AppleGetMapNSDictionary( params, &result_params );
    
    provider->onPlatformInit( result_params );
}

- (void) OnRealName:(NSDictionary*)params {
    LOGGER_MESSAGE( "OnRealName params: %s"
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }

    Mengine::MARSDKResultParams result_params;
    Mengine::Helper::AppleGetMapNSDictionary( params, &result_params );
    
    provider->onRealName( result_params );
}

- (void) OnEventWithCode:(int)code msg: (NSString*)msg {
    LOGGER_MESSAGE( "OnEventWithCode code:%d msg:%s"
        , code
        , [msg UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }

    const Mengine::Char * msg_str = [msg UTF8String];
    
    provider->onEventWithCode( code, msg_str );
}

- (void) OnEventCustom:(NSString*)eventName params:(NSDictionary*)params {
    LOGGER_MESSAGE( "OnEventCustom event:%s params:%s"
        , [eventName UTF8String]
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }

    const Mengine::Char * eventName_str = [eventName UTF8String];
    
    Mengine::MARSDKResultParams result_params;
    Mengine::Helper::AppleGetMapNSDictionary( params, &result_params );
    
    provider->onEventCustom( eventName_str, result_params );
}

- (void) OnUserLogin: (NSDictionary *)params {
    LOGGER_MESSAGE( "OnUserLogin params:%s"
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }
    
    Mengine::MARSDKResultParams result_params;
    Mengine::Helper::AppleGetMapNSDictionary( params, &result_params );
    
    provider->onUserLogin( result_params );
}

- (void) OnUserLogout: (NSDictionary *)params {
    LOGGER_MESSAGE( "OnUserLogout params:%s"
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }
    
    Mengine::MARSDKResultParams result_params;
    Mengine::Helper::AppleGetMapNSDictionary( params, &result_params );
    
    provider->onUserLogout( result_params );
}

- (void) OnPayPaid: (NSDictionary *)params {
    LOGGER_MESSAGE( "OnPayPaid params:%s"
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = self.m_service->getProvider();
    
    if( provider == nullptr )
    {
        return;
    }
    
    Mengine::MARSDKResultParams result_params;
    Mengine::Helper::AppleGetMapNSDictionary( params, &result_params );
    
    provider->onPayPaid( result_params );
}

@end
