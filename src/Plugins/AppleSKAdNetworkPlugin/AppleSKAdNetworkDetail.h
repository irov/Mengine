#pragma once

#import "Environment/Apple/AppleIncluder.h"

@interface AppleSKAdNetworkDetail : NSObject

+ (void)registerAppForAdNetworkAttribution:(NSString *)coarseValue
                         completionHandler:(void (^)(NSError *error))completion;

+ (void)updatePostbackConversionValue:(NSInteger)fineValue
                          coarseValue:(NSString *)coarseValue
                           lockWindow:(BOOL)lockWindow
                    completionHandler:(void (^)(NSError *error))completion;

+ (BOOL)isPostbackConversionUnknownError:(NSError *)error;
@end
