#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "iOSMARSDKInterface.h"
#import "iOSMARSDKDelegate.h"
#import "iOSMARSDKAdRewardedDelegate.h"

@interface iOSMARSDKPlugin : NSObject<iOSPluginInterface, iOSMARSDKInterface>

@property (nonatomic, strong) id<iOSMARSDKProviderInterface> m_provider;
@property (nonatomic, strong) iOSMARSDKDelegate * m_delegate;
@property (nonatomic, strong) iOSMARSDKAdRewardedDelegate * m_adRewardedDelegate;

@end
