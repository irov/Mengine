#pragma once

#include "AppleMARSDKInterface.h"

#import "MARSDKCore/MARSDKCore.h"

#import <Foundation/Foundation.h>

@interface AppleMARSDKDelegate : NSObject<MARSDKDelegate>

@property (nonatomic, weak) id<AppleMARSDKInterface> m_marsdk;

- (instancetype _Nonnull) initWithMARSDK:(id<AppleMARSDKInterface>)marsdk;

@end
