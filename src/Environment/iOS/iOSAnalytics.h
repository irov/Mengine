#pragma once

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/iOS/iOSAnalyticsEventCategory.h"

@interface iOSAnalytics : NSObject

+(void)event:(NSString * _Nonnull)event category:(iOSAnalyticsEventCategory)category params:(NSDictionary * _Nonnull)params;
+(void)screen:(NSString * _Nonnull)name type:(NSString * _Nonnull)type;
+(void)flush;

@end