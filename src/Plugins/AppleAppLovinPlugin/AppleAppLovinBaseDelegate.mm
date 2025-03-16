#import "AppleAppLovinBaseDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleAnalytics.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

@implementation AppleAppLovinBaseDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                          adFormat:(MAAdFormat * _Nonnull) adFormat {
    self = [super init];
    
    self.m_adUnitId = adUnitId;
    self.m_adFormat = adFormat;
    
    self.m_requestAttempt = 0;
    self.m_enumeratorRequest = 0;
    self.m_requestId = 0;
    self.m_requestTimestamp = 0.0;
    
    return self;
}

- (NSDictionary *) getMAAdParams:(MAAd *) ad {
    NSMutableDictionary * params = [NSMutableDictionary dictionary];
    
    [params setObject:ad.format.label forKey:@"format"];
    [params setObject:@[@(ad.size.width), @(ad.size.height)] forKey:@"size"];
    [params setObject:ad.adUnitIdentifier forKey:@"adUnitIdentifier"];
    [params setObject:ad.networkName forKey:@"networkName"];
    [params setObject:ad.networkPlacement forKey:@"networkPlacement"];
    [params setObject:@(ad.revenue) forKey:@"revenue"];
    [params setObject:ad.revenuePrecision forKey:@"revenuePrecision"];
    
    if (ad.creativeIdentifier != nil) {
        [params setObject:ad.creativeIdentifier forKey:@"creativeIdentifier"];
    }
    
    if (ad.placement != nil) {
        [params setObject:ad.placement forKey:@"placement"];
    }
    
    if (ad.DSPName != nil) {
        [params setObject:ad.DSPName forKey:@"DSPName"];
    }
    
    if (ad.DSPIdentifier != nil) {
        [params setObject:ad.DSPIdentifier forKey:@"DSPIdentifier"];
    }
    
    return params;
}

- (NSDictionary * _Nonnull) getMAErrorParams:(MAError * _Nonnull) error {
    NSDictionary * params = @{
        @"code": @(error.code),
        @"message": error.message,
        @"mediatedNetworkErrorCode": @(error.mediatedNetworkErrorCode),
        @"mediatedNetworkErrorMessage": error.mediatedNetworkErrorMessage,
        @"requestLatency": @(error.requestLatency)
    };
    
    return params;
}

- (NSDictionary * _Nonnull) getMARewardParams:(MAReward * _Nonnull) reward {
    NSDictionary * params = @{
        @"label": reward.label,
        @"amount": @(reward.amount)
    };

    return params;
}

- (void) log:(NSString * _Nonnull) method {
    IOS_LOGGER_MESSAGE(@"[%@] %@: adUnitId: %@ request: %ld"
        , self.m_adFormat.label
        , method
        , self.m_adUnitId
        , self.m_requestId
    );
}


- (void) log:(NSString * _Nonnull) method withParams:(NSDictionary * _Nonnull) params {
    IOS_LOGGER_MESSAGE(@"[%@] %@: adUnitId: %@ request: %ld %@"
        , self.m_adFormat.label
        , method
        , self.m_adUnitId
        , self.m_requestId
        , [NSString stringWithFormat:@"%@", params]
    );
}

- (void) log:(NSString * _Nonnull) method withMAAd:(MAAd *) ad {
    IOS_LOGGER_MESSAGE(@"[%@] %@: adUnitId: %@ request: %ld ad: %@"
        , self.m_adFormat.label
        , method
        , self.m_adUnitId
        , self.m_requestId
        , [NSString stringWithFormat:@"%@", [self getMAAdParams:ad]]
    );
}

- (void) log:(NSString * _Nonnull) method withMAAd:(MAAd * _Nonnull) ad withMAError:(MAError * _Nonnull) error {
    IOS_LOGGER_MESSAGE(@"[%@] %@: adUnitId: %@ request: %ld ad: %@ with error: %@"
        , ad.format.label
        , method
        , ad.adUnitIdentifier
        , self.m_requestId
        , [NSString stringWithFormat:@"%@", [self getMAAdParams:ad]]
        , [NSString stringWithFormat:@"%@", [self getMAErrorParams:error]]
    );
}

- (void) log:(NSString * _Nonnull) method withMAError:(MAError * _Nonnull) error {
    IOS_LOGGER_MESSAGE(@"[%@] %@: adUnitId: %@ request: %ld with error: %@"
        , self.m_adFormat.label
        , method
        , self.m_adUnitId
        , self.m_requestId
        , [NSString stringWithFormat:@"%@", [self getMAErrorParams:error]]
    );
}

- (void) log:(NSString * _Nonnull) method withMAAd:(MAAd * _Nonnull) ad withMAReward:(MAReward * _Nonnull) reward {
    IOS_LOGGER_MESSAGE(@"[%@] %@: adUnitId: %@ request: %ld with reward lable: %@ amount: %ld"
        , ad.format.label
        , method
        , ad.adUnitIdentifier
        , self.m_requestId
        , reward.label
        , reward.amount
    );
}

- (void) loadAd {
    //Empty
    
    [AppleDetail raisePureVirtualMethodException:[self class] selector:_cmd];
}

- (void) retryLoadAd {
    self.m_requestAttempt++;
    
    NSTimeInterval delaySec = pow(2, MIN(6, self.m_requestAttempt));
    
    [AppleDetail addMainQueueOperation:^{
        [NSThread sleepForTimeInterval:delaySec];
        
        [self loadAd];
    }];
}

- (NSInteger) increaseRequestId {
    self.m_requestId = self.m_enumeratorRequest++;
    self.m_requestTimestamp = [[NSDate date] timeIntervalSince1970];

    return self.m_requestId;
}

- (NSInteger) getRequestTimeMillisec {
    NSTimeInterval timestamp = [[NSDate date] timeIntervalSince1970];
    NSTimeInterval requestTime = timestamp - self.m_requestTimestamp;
    
    NSInteger requestTimeMillisec = requestTime * 1000.0;
    
    return requestTimeMillisec;
}

- (void) event:(NSString * _Nonnull)name params:(NSDictionary<NSString*, id> * _Nonnull)params {
    NSInteger requestTimeMillisec = [self getRequestTimeMillisec];
    
    NSMutableDictionary<NSString*, id> * total_params = [@{
        @"ad_unit_id": self.m_adUnitId,
        @"request_id": @(self.m_requestId),
        @"request_time": @(requestTimeMillisec),
        @"request_attempt": @(self.m_requestAttempt)
    } mutableCopy];
    
    [total_params addEntriesFromDictionary:params];
    
    [AppleAnalytics event:name params:total_params];
}

- (void) eventRevenue:(MAAd * _Nonnull) ad {
    iOSAdRevenueParam * revenue = [iOSAdRevenueParam alloc];
    
    revenue.REVENUE_PLATFORM = @"AppLovin";
    revenue.REVENUE_COUNTRY_CODE = [ALSdk shared].configuration.countryCode;
    revenue.REVENUE_PLACEMENT = ad.placement;
    revenue.REVENUE_NETWORK_PLACEMENT = ad.networkPlacement;
    revenue.REVENUE_SOURCE = ad.networkName;
    revenue.REVENUE_FORMAT = ad.format.label;
    revenue.REVENUE_UNIT = ad.adUnitIdentifier;
    revenue.REVENUE_CURRENCY = @"USD";
    revenue.REVENUE_VALUE = [NSNumber numberWithDouble:ad.revenue];
    
    [iOSDetail adRevenue:revenue];
}

@end
