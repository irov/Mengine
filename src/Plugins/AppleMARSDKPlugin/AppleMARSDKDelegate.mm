#import "AppleMARSDKDelegate.h"

#include "Environment/Apple/AppleErrorHelper.h"
#include "Environment/iOS/iOSDetail.h"

#include "Kernel/Logger.h"

@implementation AppleMARSDKDelegate

#pragma mark -

@synthesize m_service;

- (instancetype _Nonnull)initWithService: (Mengine::AppleMARSDKServiceInterface* _Nonnull)service {
    self = [super init];
    
    self.m_service = service;
    
    [[MARSDK sharedInstance] setDelegate:self];
    
    return self;
}

- (UIView *)GetView {
    UIWindow * appWindow = [UIApplication sharedApplication].delegate.window;
    
    return [appWindow.subviews objectAtIndex:0];
}

- (UIViewController *)GetViewController {
    UIViewController * rootViewController = Mengine::Helper::iOSGetRootViewController();
    
    return rootViewController;
}

- (void) OnPlatformInit: (NSDictionary *)params {
    LOGGER_INFO( "marsdk", "OnPlatformInit params[%lu]:"
        , [params count]
    );
    
#if MENGINE_LOGGER_DEBUG == 1
    [params enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL *stop) {
        LOGGER_INFO("%s = %s"
            , [[NSString stringWithFormat:@"%@", key] UTF8String]
            , [[NSString stringWithFormat:@"%@", value] UTF8String]
        );
    }];
#endif
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();

    Mengine::MARSDKResultParams result_params;
    Mengine::Helper::AppleGetMapNSDictionary( params, &result_params );
    
    provider->onPlatformInit( result_params );
}

- (void) OnRealName:(NSDictionary*)params {
    LOGGER_INFO( "marsdk", "OnRealName params[%lu]:"
        , [params count]
    );
    
#if MENGINE_LOGGER_DEBUG == 1
    [params enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL *stop) {
        LOGGER_INFO("%s = %s"
            , [[NSString stringWithFormat:@"%@", key] UTF8String]
            , [[NSString stringWithFormat:@"%@", value] UTF8String]
        );
    }];
#endif
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();

    Mengine::MARSDKResultParams result_params;
    Mengine::Helper::AppleGetMapNSDictionary( params, &result_params );
    
    provider->onRealName( result_params );
}

- (void) OnEventWithCode: (int)code msg: (NSString*)msg {
    LOGGER_INFO( "marsdk", "OnEventWithCode [%d] msg [%s]"
        , code
        , [msg UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();

    const Mengine::Char * msg_str = [msg UTF8String];
    
    provider->onEventWithCode( code, msg_str );
}

- (void) OnEventCustom:(NSString*)eventName params:(NSDictionary*)params {
    LOGGER_INFO( "marsdk", "OnEventCustom event '%s' params[%lu]:"
        , [eventName UTF8String]
        , [params count]
    );
    
#if MENGINE_LOGGER_DEBUG == 1
    [params enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL *stop) {
        LOGGER_INFO( "%s = %s"
            , [[NSString stringWithFormat:@"%@", key] UTF8String]
            , [[NSString stringWithFormat:@"%@", value] UTF8String]
        );
    }];
#endif
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();

    const Mengine::Char * eventName_str = [eventName UTF8String];
    
    Mengine::MARSDKResultParams result_params;
    Mengine::Helper::AppleGetMapNSDictionary( params, &result_params );
    
    provider->onEventCustom( eventName_str, result_params );
}

- (void) OnUserLogin: (NSDictionary *)params {
    LOGGER_INFO( "marsdk", "OnUserLogin params[%lu]:"
        , [params count]
    );
    
#if MENGINE_LOGGER_DEBUG == 1
    [params enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL *stop) {
        LOGGER_INFO( "marsdk", "%s = %s"
            , [[NSString stringWithFormat:@"%@", key] UTF8String]
            , [[NSString stringWithFormat:@"%@", value] UTF8String]
        );
    }];
#endif
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    Mengine::MARSDKResultParams result_params;
    Mengine::Helper::AppleGetMapNSDictionary( params, &result_params );
    
    provider->onUserLogin( result_params );
}

- (void) OnUserLogout: (NSDictionary *)params {
    LOGGER_INFO( "marsdk", "OnUserLogout params[%lu]:"
        , [params count]
    );
    
#if MENGINE_LOGGER_DEBUG == 1
    [params enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL *stop) {
        LOGGER_INFO( "marsdk", "%s = %s"
            , [[NSString stringWithFormat:@"%@", key] UTF8String]
            , [[NSString stringWithFormat:@"%@", value] UTF8String]
        );
    }];
#endif
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    Mengine::MARSDKResultParams result_params;
    Mengine::Helper::AppleGetMapNSDictionary( params, &result_params );
    
    provider->onUserLogout( result_params );
}

- (void) OnPayPaid: (NSDictionary *)params {
    LOGGER_INFO( "marsdk", "OnPayPaid params[%lu]:"
        , [params count]
    );
    
#if MENGINE_LOGGER_DEBUG == 1
    [params enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL *stop) {
        LOGGER_INFO( "marsdk", "%s = %s"
            , [[NSString stringWithFormat:@"%@", key] UTF8String]
            , [[NSString stringWithFormat:@"%@", value] UTF8String]
        );
    }];
#endif
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    Mengine::MARSDKResultParams result_params;
    Mengine::Helper::AppleGetMapNSDictionary( params, &result_params );
    
    provider->onPayPaid( result_params );
}

@end
