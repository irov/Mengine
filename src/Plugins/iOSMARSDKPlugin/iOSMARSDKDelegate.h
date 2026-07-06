#pragma once

#include "iOSMARSDKInterface.h"

#import "MARSDKCore/MARSDKCore.h"

#import <Foundation/Foundation.h>

@interface iOSMARSDKDelegate : NSObject<MARSDKDelegate>

@property (nonatomic, weak) id<iOSMARSDKInterface> m_marsdk;

- (instancetype _Nonnull) initWithMARSDK:(id<iOSMARSDKInterface>)marsdk;

@end
