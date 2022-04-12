#import "AppleMARSDKDelegate.h"

@implementation AppleMARSDKDelegate

#pragma mark -

@synthesize m_callbacks;

- (instancetype)initWithCallbacks:( id<AppleMARSDKCallbacksProtocol> * _Nonnull) callbacks {
    self = [super init];
    
    m_callbacks = callbacks;
    
    [MARSDK sharedInstance].delegate = self;
    
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
    //ToDo
}

- (void) OnRealName:(NSDictionary*)params {
    //ToDo
}

- (void) OnEventWithCode: (int)code msg: (NSString*)msg {
    //ToDo
}

- (void) OnEventCustom:(NSString*)eventName params:(NSDictionary*)params {
    //ToDo
}

- (BOOL) login; {
    [[MARSDK sharedInstance] login];
    return true;
}

- (void) OnUserLogin: (NSDictionary *)params {
    [m_callbacks onUserLogin:params];
}

- (BOOL) logout {
    [[MARSDK sharedInstance] logout];
    return true;
}

- (void) OnUserLogout: (NSDictionary *)params {
    [m_callbacks onUserLogout:params];
}

- (void) switchAccount {
    [[MARSDK sharedInstance] switchAccount];
}

- (void) submitExtendedData:(MARUserExtraData*_Nonnull)extraData {
    [[MARSDK sharedInstance] submitExtraData:extraData];
}

- (void) submitPayementData:(MARProductInfo* _Nonnull)productInfo {
    [[MARSDK sharedInstance] pay:productInfo];
}

- (void) OnPayPaid: (NSDictionary *)params {
    [m_callbacks onPayPaid:params];
}

@end
