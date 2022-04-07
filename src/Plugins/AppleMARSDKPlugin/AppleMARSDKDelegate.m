#import "AppleMARSDKDelegate.h"

@implementation AppleMARSDKDelegate

#pragma mark -

@synthesize m_callbacks;

- (instancetype _Nonnull)initWithLogIn:(MARSDKCb _Nonnull) logIn logOut:(MARSDKCb _Nonnull) logOut onPayPaid:(MARSDKCb _Nonnull) onPayPaid{
    self = [super init];
    if (self) {
        m_callbacks = [[MARSDKCallbacks alloc] initWithLogIn:logIn logOut:logOut onPayPaid:onPayPaid];
        [MARSDK sharedInstance].delegate = self;
    }
    return self;
}

- (UIView *)GetView {
    UIWindow * appWindow = [UIApplication sharedApplication].delegate.window;
    return  [appWindow.subviews objectAtIndex:0];
}

- (UIViewController *)GetViewController {
    UIWindow * appWindow = [UIApplication sharedApplication].delegate.window;
    return  appWindow.rootViewController;
}

- (void) OnPlatformInit: (NSDictionary *)params{}
- (void) OnRealName:(NSDictionary*)params{}
- (void) OnEventWithCode: (int)code msg: (NSString*)msg{}
- (void) OnEventCustom:(NSString*)eventName params:(NSDictionary*)params{}


- (BOOL) login;{
    [[MARSDK sharedInstance] login];
    return true;
}

- (void) OnUserLogin: (NSDictionary *)params{
    [m_callbacks onUserLogin:params];
}

- (BOOL) logout{
    [[MARSDK sharedInstance] logout];
    return true;
}

- (void) OnUserLogout: (NSDictionary *)params{
    [m_callbacks onUserLogout:params];
}

- (void) switchAccount{
    [[MARSDK sharedInstance] switchAccount];
}

- (void) submitExtendedData:(MARUserExtraData*_Nonnull)extraData {
    [[MARSDK sharedInstance] submitExtraData:extraData];
}

- (void) submitPayementData:(MARProductInfo* _Nonnull)productInfo{
    [[MARSDK sharedInstance].defaultPay pay:productInfo];
}

- (void) OnPayPaid: (NSDictionary *)params{
    [m_callbacks onPayPaid:params];
}

@end
