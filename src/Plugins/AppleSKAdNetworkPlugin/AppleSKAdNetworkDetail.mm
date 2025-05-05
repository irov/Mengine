#import "AppleSKAdNetworkDetail.h"

#import <StoreKit/SKAdNetwork.h>

@implementation AppleSKAdNetworkDetail

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
                                       coarseValue:[AppleSKAdNetworkDetail getSKAdNetworkCoarseConversionValue:coarseValue]
                                        lockWindow:NO
                                 completionHandler:completion];
    } else if (@available(iOS 15.4, *)) {
        [SKAdNetwork updatePostbackConversionValue:0
                                 completionHandler:completion];
    } else if (@available(iOS 11.3, *)) {
        [SKAdNetwork registerAppForAdNetworkAttribution];
        completion(nil);
    } else {
        NSDictionary * userInfo = @{
            NSLocalizedDescriptionKey: @"SKAdNetwork's 'registerAppForAdNetworkAttribution' method is not available for this operating system version"
        };

        NSError * error = [NSError errorWithDomain:@"com.mengine.skadnetwork"
                                              code:1001
                                          userInfo:userInfo];
                                          
        completion(error);
    }
}

+ (void)updatePostbackConversionValue:(NSInteger)fineValue
                          coarseValue:(NSString *)coarseValue
                           lockWindow:(BOOL)lockWindow
                    completionHandler:(void (^)(NSError *error))completion {
    if (@available(iOS 16.1, *)) {
        [SKAdNetwork updatePostbackConversionValue:fineValue
                                       coarseValue:[AppleSKAdNetworkDetail getSKAdNetworkCoarseConversionValue:coarseValue]
                                        lockWindow:lockWindow
                                 completionHandler:completion];
    } else if (@available(iOS 15.4, *)) {
        [SKAdNetwork updatePostbackConversionValue:fineValue
                                 completionHandler:completion];
    } else if (@available(iOS 14.0, *)) {
        [SKAdNetwork updateConversionValue:fineValue];
        completion(nil);
    } else {
        NSDictionary * userInfo = @{
            NSLocalizedDescriptionKey: @"SKAdNetwork's 'updateConversionValue' method is not available for this operating system version"
        };

        NSError * error = [NSError errorWithDomain:@"com.mengine.skadnetwork"
                                              code:1001
                                          userInfo:userInfo];
        
        completion(error);
    }
}

@end
