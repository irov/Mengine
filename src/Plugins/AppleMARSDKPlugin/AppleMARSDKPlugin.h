#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "AppleMARSDKInterface.h"
#import "AppleMARSDKDelegate.h"
#import "AppleMARSDKAdRewardedDelegate.h"

@interface AppleMARSDKPlugin : NSObject<iOSPluginInterface, AppleMARSDKInterface>

@property (nonatomic, strong) id<AppleMARSDKProviderInterface> m_provider;
@property (nonatomic, strong) AppleMARSDKDelegate * m_delegate;
@property (nonatomic, strong) AppleMARSDKAdRewardedDelegate * m_adRewardedDelegate;

@end
