#import "AppleAppLovinBaseDelegate.h"

#include "Kernel/Logger.h"

@implementation AppleAppLovinBaseDelegate

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                          adFormat:(MAAdFormat * _Nonnull) adFormat
                                         analytics:(AppleAppLovinAnalyticsService * _Nonnull) analytics {
    self = [super init];
    
    self.m_adUnitId = adUnitId;
    self.m_adFormat = adFormat;
    self.m_analytics = analytics;
    
    self.m_retryAttempt = 0;
    self.m_enumeratorRequest = 0;
    self.m_requestId = 0;
    
    return self;
}

- (NSString *) getMAAdParams:(MAAd *) ad {
    NSMutableString * params = [NSMutableString stringWithCapacity:1024];
    
    [params appendString:@"{"];
    [params appendFormat:@"\"format\": \"%@\",", ad.format.label];
    [params appendFormat:@", \"size\": [%f, %f],", ad.size.width, ad.size.height];
    [params appendFormat:@", \"adUnitIdentifier\": \"%@\",", ad.adUnitIdentifier];
    [params appendFormat:@", \"networkName\": \"%@\",", ad.networkName];
    [params appendFormat:@", \"networkPlacement\": \"%@\",", ad.networkPlacement];
    [params appendFormat:@", \"revenue\": \"%lf\",", ad.revenue];
    [params appendFormat:@", \"revenuePrecision\": \"%@\",", ad.revenuePrecision];
    
    if (ad.creativeIdentifier) {
        [params appendFormat:@", \"creativeIdentifier\": \"%@\",", ad.creativeIdentifier];
    }
    
    if (ad.adReviewCreativeIdentifier) {
        [params appendFormat:@", \"adReviewCreativeIdentifier\": \"%@\",", ad.adReviewCreativeIdentifier];
    }
    
    if (ad.placement) {
        [params appendFormat:@", \"placement\": \"%@\",", ad.placement];
    }
    
    if (ad.DSPName) {
        [params appendFormat:@", \"DSPName\": \"%@\",", ad.DSPName];
    }
    
    if (ad.DSPIdentifier) {
        [params appendFormat:@", \"DSPIdentifier\": \"%@\"", ad.DSPIdentifier];
    }
    
    [params appendString:@"}"];
    
    return params;
}

- (NSString * _Nonnull) getMAErrorParams:(MAError * _Nonnull) error {
    NSMutableString * params = [NSMutableString stringWithCapacity:1024];
    
    [params appendString:@"{"];
    [params appendFormat:@"\"code\": \"%ld\",", error.code];
    [params appendFormat:@"\"message\": \"%@\",", error.message];
    [params appendFormat:@"\"mediated_network_error_code\": \"%ld\",", error.mediatedNetworkErrorCode];
    [params appendFormat:@"\"mediated_network_error_message\": \"%@\",", error.mediatedNetworkErrorMessage];
    [params appendFormat:@"\"latency\": \"%lf\"", error.requestLatency];
    [params appendString:@"}"];

    return params;
}

- (NSString * _Nonnull) getMARewardParams:(MAReward * _Nonnull) reward {
    NSMutableString * params = [NSMutableString stringWithCapacity:256];
    
    [params appendString:@"{"];
    [params appendFormat:@"\"label\": \"%@\",", reward.label];
    [params appendFormat:@"\"amount\": \"%ld\"", reward.amount];
    [params appendString:@"}"];

    return params;
}

- (void) log:(NSString * _Nonnull) callback {
    LOGGER_MESSAGE( "[%s] %s: adUnitId: %s request: %ld"
        , [self.m_adFormat.label UTF8String]
        , [callback UTF8String]
        , [self.m_adUnitId UTF8String]
        , self.m_requestId
    );
}


- (void) log:(NSString * _Nonnull) callback withParams:(NSDictionary * _Nonnull) params {
    LOGGER_MESSAGE( "[%s] %s: adUnitId: %s request: %ld %s"
        , [self.m_adFormat.label UTF8String]
        , [callback UTF8String]
        , [self.m_adUnitId UTF8String]
        , self.m_requestId
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );
}

- (void) log:(NSString * _Nonnull) callback withMAAd:(MAAd *) ad {
    LOGGER_MESSAGE( "[%s] %s: adUnitId: %s request: %ld ad: %s"
        , [self.m_adFormat.label UTF8String]
        , [callback UTF8String]
        , [self.m_adUnitId UTF8String]
        , self.m_requestId
        , [[self getMAAdParams:ad] UTF8String]
    );
}

- (void) log:(NSString * _Nonnull) callback withMAAd:(MAAd * _Nonnull) ad withMAError:(MAError * _Nonnull) error {
    LOGGER_MESSAGE( "[%s] %s: adUnitId: %s request: %ld ad: %s with error: %s"
        , [self.m_adFormat.label UTF8String]
        , [callback UTF8String]
        , [self.m_adUnitId UTF8String]
        , self.m_requestId
        , [[self getMAAdParams:ad] UTF8String]
        , [[self getMAErrorParams:error] UTF8String]
    );
}

- (void) log:(NSString * _Nonnull) callback withMAError:(MAError * _Nonnull) error {
    LOGGER_MESSAGE( "[%s] %s: adUnitId: %s request: %ld with error: %s"
        , [self.m_adFormat.label UTF8String]
        , [callback UTF8String]
        , [self.m_adUnitId UTF8String]
        , self.m_requestId
        , [[self getMAErrorParams:error] UTF8String]
    );
}

- (void) log:(NSString * _Nonnull) callback withMAAd:(MAAd * _Nonnull) ad withMAReward:(MAReward * _Nonnull) reward {
    LOGGER_MESSAGE( "[%s] %s: adUnitId: %s request: %ld with reward lable: %s amount: %ld"
        , [self.m_adFormat.label UTF8String]
        , [callback UTF8String]
        , [self.m_adUnitId UTF8String]
        , self.m_requestId
        , [reward.label UTF8String]
        , reward.amount
    );
}

- (void) loadAd {
    //Empty
}

- (void) retryLoadAd {
    self.m_retryAttempt++;
    
    NSInteger delaySec = pow(2, MIN(6, self.m_retryAttempt));
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, delaySec * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self loadAd];
    });
}

@end
