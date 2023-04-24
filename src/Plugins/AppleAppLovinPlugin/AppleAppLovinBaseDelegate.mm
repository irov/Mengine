#import "AppleAppLovinBaseDelegate.h"

@implementation AppleAppLovinBaseDelegate

- (NSString *) getMAAdParams:(MAAd *) ad {
    NSMutableString * params = [NSMutableString stringWithCapacity:1024];
    
    [params appendString:@"{"];
    [params appendFormat:@"\"format\": \"%@\"", ad.format.label];
    [params appendFormat:@", \"size\": [%f, %f]", ad.size.width, ad.size.height];
    [params appendFormat:@", \"adUnitIdentifier\": \"%@\"", ad.adUnitIdentifier];
    [params appendFormat:@", \"networkName\": \"%@\"", ad.networkName];
    [params appendFormat:@", \"networkPlacement\": \"%@\"", ad.networkPlacement];
    [params appendFormat:@", \"revenue\": \"%lf\"", ad.revenue];
    [params appendFormat:@", \"revenuePrecision\": \"%@\"", ad.revenuePrecision];
    
    if(ad.creativeIdentifier) {
        [params appendFormat:@", \"creativeIdentifier\": \"%@\"", ad.creativeIdentifier];
    }
    
    if(ad.adReviewCreativeIdentifier) {
        [params appendFormat:@", \"adReviewCreativeIdentifier\": \"%@\"", ad.adReviewCreativeIdentifier];
    }
    
    if(ad.placement) {
        [params appendFormat:@", \"placement\": \"%@\"", ad.placement];
    }
    
    if(ad.DSPName) {
        [params appendFormat:@", \"DSPName\": \"%@\"", ad.DSPName];
    }
    
    if(ad.DSPIdentifier) {
        [params appendFormat:@", \"DSPIdentifier\": \"%@\"", ad.DSPIdentifier];
    }
    
    [params appendString:@"}"];
    
    return params;
}

- (NSString * _Nonnull) getMAErrorParams:(MAError * _Nonnull) error {
    NSMutableString * params = [NSMutableString stringWithCapacity:1024];
    
    [params appendString:@"{"];
    [params appendFormat:@"\"code\": \"%ld\"", error.code];
    [params appendFormat:@"\"message\": \"%@\"", error.message];
    [params appendFormat:@"\"mediated_network_error_code\": \"%ld\"", error.mediatedNetworkErrorCode];
    [params appendFormat:@"\"mediated_network_error_message\": \"%@\"", error.mediatedNetworkErrorMessage];
    [params appendFormat:@"\"latency\": \"%lf\"", error.requestLatency];
    [params appendString:@"}"];

    return params;
}

- (NSString * _Nonnull) getMARewardParams:(MAReward * _Nonnull) reward {
    NSMutableString * params = [NSMutableString stringWithCapacity:256];
    
    [params appendString:@"{"];
    [params appendFormat:@"\"label\": \"%@\"", reward.label];
    [params appendFormat:@"\"amount\": \"%ld\"", reward.amount];
    [params appendString:@"}"];

    return params;
}

@end
