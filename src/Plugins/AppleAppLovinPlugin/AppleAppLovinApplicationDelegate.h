#import "Environment/iOS/UIPluginApplicationDelegateInterface.h"

#import <AppLovinSDK/AppLovinSDK.h>

@interface AppleAppLovinApplicationDelegate : NSObject<UIPluginApplicationDelegateInterface>

+ (ALSdk * _Nonnull) AppLovinSdk;
+ (void) setAppLovinSdk:(ALSdk * _Nonnull) value;

@end


