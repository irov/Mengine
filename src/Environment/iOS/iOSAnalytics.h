#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface iOSAnalytics : NSObject

+(void)event:(NSString * _Nonnull)event params:(NSDictionary * _Nonnull)params;
+(void)screen:(NSString * _Nonnull)name type:(NSString * _Nonnull)type;
+(void)flush;

@end
