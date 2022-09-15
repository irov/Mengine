#import "AppleMARSDKDelegate.h"

#include "Environment/Apple/AppleErrorHelper.h"

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
    UIWindow * appWindow = [UIApplication sharedApplication].delegate.window;
    
    return appWindow.rootViewController;
}

- (void) OnPlatformInit: (NSDictionary *)params {
    LOGGER_INFO( "marsdk", "OnPlatformInit params:" );
    
#if MENGINE_LOGGER_DEBUG == 1
    [params enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL *stop) {
        LOGGER_INFO("%s = %s"
            , [[NSString stringWithFormat:@"%@", key] UTF8String]
            , [[NSString stringWithFormat:@"%@", value] UTF8String]
        );
    }];
#endif
    
    //ToDo
}

- (void) OnRealName:(NSDictionary*)params {
    LOGGER_INFO( "marsdk", "OnRealName params:" );
    
#if MENGINE_LOGGER_DEBUG == 1
    [params enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL *stop) {
        LOGGER_INFO("%s = %s"
            , [[NSString stringWithFormat:@"%@", key] UTF8String]
            , [[NSString stringWithFormat:@"%@", value] UTF8String]
        );
    }];
#endif
    
    //ToDo
}

- (void) OnEventWithCode: (int)code msg: (NSString*)msg {
    LOGGER_INFO( "marsdk", "OnEventWithCode [%d] msg [%s]"
        , code
        , [msg UTF8String]
    );
    
    //ToDo
}

- (void) OnEventCustom:(NSString*)eventName params:(NSDictionary*)params {
    LOGGER_INFO( "marsdk", "OnEventCustom event '%s' params:"
        , [eventName UTF8String]
    );
    
#if MENGINE_LOGGER_DEBUG == 1
    [params enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL *stop) {
        LOGGER_INFO( "%s = %s"
            , [[NSString stringWithFormat:@"%@", key] UTF8String]
            , [[NSString stringWithFormat:@"%@", value] UTF8String]
        );
    }];
#endif
    
    //ToDo
}

- (BOOL) login {
    [[MARSDK sharedInstance] login];
    
    return true;
}

- (void) OnUserLogin: (NSDictionary *)params {
    LOGGER_INFO( "marsdk", "OnUserLogin params:");
    
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

- (BOOL) logout {
    [[MARSDK sharedInstance] logout];
    
    return true;
}

- (void) OnUserLogout: (NSDictionary *)params {
    LOGGER_INFO( "marsdk", "OnUserLogout params:");
    
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

- (void) switchAccount {
    [[MARSDK sharedInstance] switchAccount];
}

- (void) submitExtendedData:(MARUserExtraData*_Nonnull)extraData {
    [[MARSDK sharedInstance] submitExtraData:extraData];
}

- (void) submitPaymentData:(MARProductInfo* _Nonnull)productInfo {
    [[MARSDK sharedInstance] pay:productInfo];
}

- (void) OnPayPaid: (NSDictionary *)params {
    LOGGER_INFO( "marsdk", "OnPayPaid params:");
    
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
