#import "Interface/UIKitProxyApplicationDelegateInterface.h"

#import <Foundation/Foundation.h>
#import "MARSDKCore/MARSDKCore.h"

@interface AppleMARSDKApplicationDelegate : NSObject<UIKitProxyApplicationDelegateInterface>
{
}

@property (class, nonatomic, assign, readonly) BOOL hasInited;
@end
