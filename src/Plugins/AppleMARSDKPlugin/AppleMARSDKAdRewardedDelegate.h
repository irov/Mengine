#pragma once

#include "AppleMARSDKInterface.h"

#import "MARSDKCore/MARSDKCore.h"
#import "MARSDKCore/MARAd.h"

#import <Foundation/Foundation.h>

@interface AppleMARSDKAdRewardedDelegate : NSObject<MARAdRewardedDelegate>

@property (nonatomic, weak) id<AppleMARSDKInterface> m_marsdk;

- (instancetype _Nonnull) initWithMARSDK:(id<AppleMARSDKInterface>)marsdk;

@end
