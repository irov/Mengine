#import "Interface/UIKitProxyApplicationDelegateInterface.h"

#import <Foundation/Foundation.h>

@interface AppleMARSDKApplicationDelegate : NSObject<UIKitProxyApplicationDelegateInterface>
{
}

@property (class, nonatomic, assign, readonly) BOOL hasInited;
@end
