#import "AppleMARSDKCallbacks.h"


@implementation MARSDKCallbacks

- (instancetype )initWithLogIn:(MARSDKCb) logIn logOut:(MARSDKCb ) logOut onPayPaid:(MARSDKCb) onPayPaid
{
    self = [super init];
    if (self) {
        self.m_logIn = logIn;
        self.m_logOut = logOut;
        self.m_payPaid = onPayPaid;
    }
}

- (void) onUserLogin:(NSDictionary* _Nullable) params{
    self.m_logIn(params);
}

- (void) onUserLogout:(NSDictionary* _Nullable) params{
    self.m_logOut(params);
}

- (void) onPayPaid:(NSDictionary* _Nullable) params{
    self.m_payPaid(params);
}

@end
