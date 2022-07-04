#import "AppleMARSDKDelegate.h"

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
    LOGGER_INFO("marsdk", "OnPlatformInit");
    
    //ToDo
}

- (void) OnRealName:(NSDictionary*)params {
    LOGGER_INFO("marsdk", "OnRealName");
    
    //ToDo
}

- (void) OnEventWithCode: (int)code msg: (NSString*)msg {
    LOGGER_INFO("marsdk", "OnEventWithCode");
    
    //ToDo
}

- (void) OnEventCustom:(NSString*)eventName params:(NSDictionary*)params {
    LOGGER_INFO("marsdk", "OnEventCustom");
    
    //ToDo
}

- (BOOL) login {
    [[MARSDK sharedInstance] login];
    
    return true;
}

- (void) OnUserLogin: (NSDictionary *)params {
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onUserLogin();
}

- (BOOL) logout {
    [[MARSDK sharedInstance] logout];
    
    return true;
}

- (void) OnUserLogout: (NSDictionary *)params {
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onUserLogout();
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
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onPayPaid();
}

@end
