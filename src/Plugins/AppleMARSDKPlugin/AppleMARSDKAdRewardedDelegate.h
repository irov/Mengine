#pragma once

#include "AppleMARSDKInterface.h"

#import "MARSDKCore/MARSDKCore.h"
#import "MARSDKCore/MARAd.h"

#import <Foundation/Foundation.h>

@interface AppleMARSDKAdRewardedDelegate : NSObject<MARAdRewardedDelegate>

@property (assign) Mengine::AppleMARSDKServiceInterface * _Nonnull m_service;

- (instancetype _Nonnull)initWithService:(Mengine::AppleMARSDKServiceInterface* _Nonnull)service;

@end
