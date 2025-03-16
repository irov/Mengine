#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import "AppleStoreReviewInterface.h"

@interface AppleStoreReviewApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AppleStoreReviewInterface>

@end
