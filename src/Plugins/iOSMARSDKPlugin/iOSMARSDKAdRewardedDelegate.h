#pragma once

#include "iOSMARSDKInterface.h"

#import "MARSDKCore/MARSDKCore.h"
#import "MARSDKCore/MARAd.h"

#import <Foundation/Foundation.h>

@interface iOSMARSDKAdRewardedDelegate : NSObject<MARAdRewardedDelegate>

@property (nonatomic, weak) id<iOSMARSDKInterface> m_marsdk;

- (instancetype _Nonnull) initWithMARSDK:(id<iOSMARSDKInterface>)marsdk;

@end
