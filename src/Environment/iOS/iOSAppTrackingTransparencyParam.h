#pragma once

#include "Config/Config.h"

#import "Environment/iOS/iOSAppTrackingAuthorization.h"

@interface iOSAppTrackingTransparencyParam : NSObject

@property (nonatomic) EAppleAppTrackingAuthorization APPTRACKINGTRANSPARENCY_AUTHORIZATION;
@property (nonatomic) NSString * APPTRACKINGTRANSPARENCY_IDFA;

@end
