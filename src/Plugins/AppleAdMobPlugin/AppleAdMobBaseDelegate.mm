#import "AppleAdMobBaseDelegate.h"

#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSAnalytics.h"
#import "Environment/iOS/iOSLog.h"

@implementation AppleAdMobBaseDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                      advertisement:(id<AppleAdvertisementInterface> _Nonnull)advertisement {
    self = [super init];
    
    self.m_adUnitId = adUnitId;
    self.m_advertisement = advertisement;
    
    self.m_requestAttempt = 0;
    self.m_enumeratorRequest = 0;
    self.m_requestId = 0;
    self.m_requestTimestamp = 0.0;
    
    return self;
}

- (NSDictionary * _Nonnull) getGADResponseInfoParams:(GADResponseInfo * _Nullable) responseInfo {
    NSMutableDictionary * params = [NSMutableDictionary dictionary];
    
    if (responseInfo != nil) {
        if (responseInfo.responseIdentifier != nil) {
            [params setObject:responseInfo.responseIdentifier forKey:@"responseIdentifier"];
        }
        if (responseInfo.loadedAdNetworkResponseInfo != nil) {
            GADAdNetworkResponseInfo * loadedInfo = responseInfo.loadedAdNetworkResponseInfo;
            if (loadedInfo.adNetworkClassName != nil) {
                [params setObject:loadedInfo.adNetworkClassName forKey:@"adNetworkClassName"];
            }
            NSDictionary<NSString *, id> * dictionaryRepresentation = loadedInfo.dictionaryRepresentation;
            if (dictionaryRepresentation != nil) {
                [params setObject:dictionaryRepresentation forKey:@"dictionaryRepresentation"];
            }
        }
    }
    
    return params;
}

- (NSDictionary * _Nonnull) getGADAdErrorParams:(NSError * _Nonnull) error {
    NSMutableDictionary * params = [NSMutableDictionary dictionary];
    
    [params setObject:@(error.code) forKey:@"code"];
    
    if (error.localizedDescription != nil) {
        [params setObject:error.localizedDescription forKey:@"message"];
    }
    
    if (error.userInfo != nil) {
        NSDictionary * userInfo = error.userInfo;
        
        if (userInfo[@"NSUnderlyingError"] != nil) {
            NSError * underlyingError = userInfo[@"NSUnderlyingError"];
            [params setObject:@(underlyingError.code) forKey:@"underlyingErrorCode"];
            if (underlyingError.localizedDescription != nil) {
                [params setObject:underlyingError.localizedDescription forKey:@"underlyingErrorMessage"];
            }
        }
    }
    
    return params;
}

- (void) log:(NSString * _Nonnull) method {
    IOS_LOGGER_INFO(@"[AdMob] %@: adUnitId: %@ request: %ld"
        , method
        , self.m_adUnitId
        , self.m_requestId
    );
}

- (void) log:(NSString * _Nonnull) method withParams:(NSDictionary * _Nonnull) params {
    IOS_LOGGER_INFO(@"[AdMob] %@: adUnitId: %@ request: %ld %@"
        , method
        , self.m_adUnitId
        , self.m_requestId
        , [NSString stringWithFormat:@"%@", params]
    );
}

- (void) log:(NSString * _Nonnull) method withError:(NSError * _Nonnull) error {
    IOS_LOGGER_INFO(@"[AdMob] %@: adUnitId: %@ request: %ld with error: %@"
        , method
        , self.m_adUnitId
        , self.m_requestId
        , [NSString stringWithFormat:@"%@", [self getGADAdErrorParams:error]]
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
        [self loadAd];
    } afterSeconds:delaySec];
}

- (NSInteger) increaseRequestId {
    self.m_requestId = self.m_enumeratorRequest++;
    self.m_requestTimestamp = [[NSDate date] timeIntervalSince1970];

    return self.m_requestId;
}

- (NSInteger) getRequestTimeMillisec {
    NSTimeInterval timestamp = [[NSDate date] timeIntervalSince1970];
    NSTimeInterval requestTime = timestamp - self.m_requestTimestamp;
    
    NSInteger requestTimeMillisec = (NSInteger)(requestTime * 1000.0);
    
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
    
    [iOSAnalytics eventSystem:name params:total_params];
}

- (GADRequest *) createAdRequest {
    GADRequest * request = [GADRequest request];
    request.requestAgent = @"Mengine";
    
    return request;
}

@end

