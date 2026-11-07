#import "iOSSKAdNetworkDetail.h"

#import <StoreKit/SKAdNetwork.h>
#import <StoreKit/SKANError.h>

@implementation iOSSKAdNetworkDetail

+ (NSString *)getSKAdNetworkCoarseConversionValue:(NSString *)coarseValue {
    if (@available(iOS 16.1, *)) {
        if ([coarseValue isEqualToString:@"low"]) {
            return SKAdNetworkCoarseConversionValueLow;
        } else if ([coarseValue isEqualToString:@"medium"]) {
            return SKAdNetworkCoarseConversionValueMedium;
        } else if ([coarseValue isEqualToString:@"high"]) {
            return SKAdNetworkCoarseConversionValueHigh;
        } else {
            return nil;
        }
    } else {
        return nil;
    }
}

+ (void)registerAppForAdNetworkAttribution:(NSString *)coarseValue
                         completionHandler:(void (^)(NSError *error))completion {
    if (@available(iOS 16.1, *)) {
        NSString *coarseConversionValue = [iOSSKAdNetworkDetail getSKAdNetworkCoarseConversionValue:coarseValue];
        [SKAdNetwork updatePostbackConversionValue:0
                                       coarseValue:coarseConversionValue
                                        lockWindow:NO
                                 completionHandler:completion];
    } else {
        [SKAdNetwork updatePostbackConversionValue:0
                                 completionHandler:completion];
    }
}

+ (void)updatePostbackConversionValue:(NSInteger)fineValue
                          coarseValue:(NSString *)coarseValue
                           lockWindow:(BOOL)lockWindow
                    completionHandler:(void (^)(NSError *error))completion {
    if (@available(iOS 16.1, *)) {
        NSString *coarseConversionValue = [iOSSKAdNetworkDetail getSKAdNetworkCoarseConversionValue:coarseValue];
        [SKAdNetwork updatePostbackConversionValue:fineValue
                                       coarseValue:coarseConversionValue
                                        lockWindow:lockWindow
                                 completionHandler:completion];
    } else {
        [SKAdNetwork updatePostbackConversionValue:fineValue
                                 completionHandler:completion];
    }
}

+ (BOOL)isPostbackConversionUnknownError:(NSError *)error {
    if (error.code == SKANErrorUnknown) {
        return YES;
    }

    return NO;
}

@end
