#import "AppleMARSDKCallbacksProtocol.h"

#import "MARSDKCore/MARSDKCore.h"

#import <Foundation/Foundation.h>

@interface AppleMARSDKDelegate : NSObject<MARSDKDelegate>

@property (assign) id<AppleMARSDKCallbacksProtocol> * _Nonnull m_callbacks;

- (instancetype _Nonnull)initWithCallbacks:(id<AppleMARSDKCallbacksProtocol> * _Nonnull) callbacks;

- (BOOL) login;
- (BOOL) logout;
- (void) switchAccount;
- (void) submitExtendedData:(MARUserExtraData* _Nonnull)extraData;
- (void) submitPayementData:(MARProductInfo* _Nonnull)productInfo;

@end
