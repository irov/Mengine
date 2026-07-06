#pragma once

#import "iOSAdMobInterface.h"

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/iOS/iOSAdRevenueParam.h"

#import "Plugins/iOSAdvertisementPlugin/iOSAdvertisementInterface.h"

#import <GoogleMobileAds/GoogleMobileAds.h>

@interface iOSAdMobBaseDelegate : NSObject

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                      advertisement:(id<iOSAdvertisementInterface> _Nonnull)advertisement;

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

- (void) eventRevenue:(GADAdValue * _Nonnull)adValue responseInfo:(GADResponseInfo * _Nullable)responseInfo placement:(NSString * _Nullable)placement format:(NSString * _Nonnull)format;

- (GADRequest * _Nonnull) createAdRequest;

- (void) setAdFreeze:(BOOL)freeze;

@property (nonatomic, strong) NSString * _Nonnull m_adUnitId;
@property (nonatomic, strong) id<iOSAdvertisementInterface> _Nonnull m_advertisement;

@property (nonatomic, assign) NSInteger m_enumeratorRequest;
@property (nonatomic, assign) NSInteger m_requestId;
@property (nonatomic, assign) NSInteger m_requestAttempt;
@property (nonatomic, assign) NSTimeInterval m_requestTimestamp;

@end
