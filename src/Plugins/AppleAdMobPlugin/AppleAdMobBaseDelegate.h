#pragma once

#import "AppleAdMobInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#import "Plugins/AppleAdvertisementPlugin/AppleAdvertisementInterface.h"

#import <GoogleMobileAds/GoogleMobileAds.h>

@interface AppleAdMobBaseDelegate : NSObject

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                      advertisement:(id<AppleAdvertisementInterface> _Nonnull)advertisement;

- (void) loadAd;
- (void) retryLoadAd;
- (NSInteger) increaseRequestId;
- (NSInteger) getRequestTimeMillisec;

- (NSDictionary * _Nonnull) getGADResponseInfoParams:(GADResponseInfo * _Nullable) responseInfo;
- (NSDictionary * _Nonnull) getGADAdErrorParams:(NSError * _Nonnull) error;

- (void) log:(NSString * _Nonnull) callback;
- (void) log:(NSString * _Nonnull) callback withParams:(NSDictionary * _Nonnull) params;
- (void) log:(NSString * _Nonnull) callback withError:(NSError * _Nonnull) error;

- (void) event:(NSString * _Nonnull)name params:(NSDictionary<NSString*, id> * _Nonnull)params;

@property (nonatomic, strong) NSString * _Nonnull m_adUnitId;
@property (nonatomic, strong) id<AppleAdvertisementInterface> _Nonnull m_advertisement;

@property (nonatomic, assign) NSInteger m_enumeratorRequest;
@property (nonatomic, assign) NSInteger m_requestId;
@property (nonatomic, assign) NSInteger m_requestAttempt;
@property (nonatomic, assign) NSTimeInterval m_requestTimestamp;

@end

