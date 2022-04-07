#import <Foundation/Foundation.h>

@protocol AppleMARSDKCallbacksProtocol <NSObject>
- (void) onUserLogin:(NSDictionary* _Nullable) params;
- (void) onUserLogout:(NSDictionary* _Nullable) params;
- (void) onPayPaid:(NSDictionary* _Nullable) params;
@end
