#import "AppleAppLovinBaseDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSDetail.h"

#include "Kernel/Logger.h"

@implementation AppleAppLovinBaseDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                          adFormat:(MAAdFormat * _Nonnull) adFormat {
    self = [super init];
    
    self.m_adUnitId = adUnitId;
    self.m_adFormat = adFormat;
    
    self.m_retryAttempt = 0;
    self.m_enumeratorRequest = 0;
    self.m_requestId = 0;
    
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
    
    if (ad.creativeIdentifier) {
        [params setObject:ad.creativeIdentifier forKey:@"creativeIdentifier"];
    }
    
    if (ad.adReviewCreativeIdentifier) {
        [params setObject:ad.adReviewCreativeIdentifier forKey:@"adReviewCreativeIdentifier"];
    }
    
    if (ad.placement) {
        [params setObject:ad.placement forKey:@"placement"];
    }
    
    if (ad.DSPName) {
        [params setObject:ad.DSPName forKey:@"DSPName"];
    }
    
    if (ad.DSPIdentifier) {
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
    LOGGER_MESSAGE( "[%s] %s: adUnitId: %s request: %ld"
        , [self.m_adFormat.label UTF8String]
        , [method UTF8String]
        , [self.m_adUnitId UTF8String]
        , self.m_requestId
    );
}


- (void) log:(NSString * _Nonnull) method withParams:(NSDictionary * _Nonnull) params {
    LOGGER_MESSAGE( "[%s] %s: adUnitId: %s request: %ld %s"
        , [self.m_adFormat.label UTF8String]
        , [method UTF8String]
        , [self.m_adUnitId UTF8String]
        , self.m_requestId
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );
}

- (void) log:(NSString * _Nonnull) method withMAAd:(MAAd *) ad {
    LOGGER_MESSAGE( "[%s] %s: adUnitId: %s request: %ld ad: %s"
        , [self.m_adFormat.label UTF8String]
        , [method UTF8String]
        , [self.m_adUnitId UTF8String]
        , self.m_requestId
        , [[NSString stringWithFormat:@"%@", [self getMAAdParams:ad]] UTF8String]
    );
}

- (void) log:(NSString * _Nonnull) method withMAAd:(MAAd * _Nonnull) ad withMAError:(MAError * _Nonnull) error {
    LOGGER_MESSAGE( "[%s] %s: adUnitId: %s request: %ld ad: %s with error: %s"
        , [ad.format.label UTF8String]
        , [method UTF8String]
        , [ad.adUnitIdentifier UTF8String]
        , self.m_requestId
        , [[NSString stringWithFormat:@"%@", [self getMAAdParams:ad]] UTF8String]
        , [[NSString stringWithFormat:@"%@", [self getMAErrorParams:error]] UTF8String]
    );
}

- (void) log:(NSString * _Nonnull) method withMAError:(MAError * _Nonnull) error {
    LOGGER_MESSAGE( "[%s] %s: adUnitId: %s request: %ld with error: %s"
        , [self.m_adFormat.label UTF8String]
        , [method UTF8String]
        , [self.m_adUnitId UTF8String]
        , self.m_requestId
        , [[NSString stringWithFormat:@"%@", [self getMAErrorParams:error]] UTF8String]
    );
}

- (void) log:(NSString * _Nonnull) method withMAAd:(MAAd * _Nonnull) ad withMAReward:(MAReward * _Nonnull) reward {
    LOGGER_MESSAGE( "[%s] %s: adUnitId: %s request: %ld with reward lable: %s amount: %ld"
        , [ad.format.label UTF8String]
        , [method UTF8String]
        , [ad.adUnitIdentifier UTF8String]
        , self.m_requestId
        , [reward.label UTF8String]
        , reward.amount
    );
}

- (void) loadAd {
    //Empty
    
    [AppleDetail raisePureVirtualMethodException:[self class] selector:_cmd];
}

- (void) retryLoadAd {
    self.m_retryAttempt++;
    
    NSTimeInterval delaySec = pow(2, MIN(6, self.m_retryAttempt));
    
    [iOSDetail addMainQueueOperation:^{
        [NSThread sleepForTimeInterval:delaySec];
        
        [self loadAd];
    }];
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
