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
        [SKAdNetwork updatePostbackConversionValue:0
                                       coarseValue:[iOSSKAdNetworkDetail getSKAdNetworkCoarseConversionValue:coarseValue]
                                        lockWindow:NO
                                 completionHandler:completion];
    } else if (@available(iOS 15.4, *)) {
        [SKAdNetwork updatePostbackConversionValue:0
                                 completionHandler:completion];
    } else {
        [SKAdNetwork registerAppForAdNetworkAttribution];
        completion(nil);
    }
}

+ (void)updatePostbackConversionValue:(NSInteger)fineValue
                          coarseValue:(NSString *)coarseValue
                           lockWindow:(BOOL)lockWindow
                    completionHandler:(void (^)(NSError *error))completion {
    if (@available(iOS 16.1, *)) {
        [SKAdNetwork updatePostbackConversionValue:fineValue
                                       coarseValue:[iOSSKAdNetworkDetail getSKAdNetworkCoarseConversionValue:coarseValue]
                                        lockWindow:lockWindow
                                 completionHandler:completion];
    } else if (@available(iOS 15.4, *)) {
        [SKAdNetwork updatePostbackConversionValue:fineValue
                                 completionHandler:completion];
    } else {
        [SKAdNetwork updateConversionValue:fineValue];
        completion(nil);
    }
}

+ (BOOL)isPostbackConversionUnknownError:(NSError *)error {
    if (@available(iOS 15.4, *)) {
        if (error.code == SKANErrorUnknown) {
            return YES;
        }
    }

    return NO;
}

@end
