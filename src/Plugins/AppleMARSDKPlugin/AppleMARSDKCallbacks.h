
#import <Foundation/Foundation.h>
#import "MARSDKCore/MARSDKCore.h"


typedef void(^MARSDKCb)(NSDictionary* _Nullable);

@protocol MARSDKCallbacksDelegate
- (void) onUserLogin:(NSDictionary* _Nullable) params;
- (void) onUserLogout:(NSDictionary* _Nullable) params;
- (void) onPayPaid:(NSDictionary* _Nullable) params;
@end


@interface MARSDKCallbacks:NSObject<MARSDKCallbacksDelegate>

- (instancetype _Nonnull)initWithLogIn:(MARSDKCb _Nonnull) logIn logOut:(MARSDKCb _Nonnull) logOut onPayPaid:(MARSDKCb _Nonnull) onPayPaid;

@property (assign) MARSDKCb _Nonnull  m_logIn;
@property (assign) MARSDKCb _Nonnull  m_logOut;
@property (assign) MARSDKCb _Nonnull  m_payPaid;

@end

