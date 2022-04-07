
#import <Foundation/Foundation.h>
#import "MARSDKCore/MARSDKCore.h"

#import "AppleMARSDKCallbacks.h"


@interface AppleMARSDKDelegate : NSObject<MARSDKDelegate>

@property (assign) MARSDKCallbacks* _Nonnull m_callbacks;

- (instancetype _Nonnull)initWithLogIn:(MARSDKCb _Nonnull) logIn logOut:(MARSDKCb _Nonnull) logOut onPayPaid:(MARSDKCb _Nonnull) onPayPaid;
- (BOOL) login;
- (BOOL) logout;
- (void) switchAccount;
- (void) submitExtendedData:(MARUserExtraData* _Nonnull)extraData;
- (void) submitPayementData:(MARProductInfo* _Nonnull)productInfo;

@end
