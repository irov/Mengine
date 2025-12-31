#pragma once

#import "AppleAppLovinInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#import "Plugins/AppleAdvertisementPlugin/AppleAdvertisementInterface.h"

#import <AppLovinSDK/AppLovinSDK.h>

@interface AppleAppLovinBaseDelegate : NSObject

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                           adFormat:(MAAdFormat * _Nonnull) adFormat
                                      advertisement:(id<AppleAdvertisementInterface> _Nonnull)advertisement;

- (void) loadAd;
- (void) retryLoadAd;
- (NSInteger) increaseRequestId;
- (NSInteger) getRequestTimeMillisec;

- (NSDictionary * _Nonnull) getMAAdParams:(MAAd * _Nonnull) ad;
- (NSDictionary * _Nonnull) getMAErrorParams:(MAError * _Nonnull) error;
- (NSDictionary * _Nonnull) getMARewardParams:(MAReward * _Nonnull) reward;

- (void) log:(NSString * _Nonnull) callback;
- (void) log:(NSString * _Nonnull) callback withParams:(NSDictionary * _Nonnull) params;
- (void) log:(NSString * _Nonnull) callback withMAAd:(MAAd * _Nonnull) ad;
- (void) log:(NSString * _Nonnull) callback withMAAd:(MAAd * _Nonnull) ad withMAError:(MAError * _Nonnull)error;
- (void) log:(NSString * _Nonnull) callback withMAError:(MAError * _Nonnull)error;
- (void) log:(NSString * _Nonnull) callback withMAAd:(MAAd * _Nonnull) ad withMAReward:(MAReward * _Nonnull)reward;

- (void) event:(NSString * _Nonnull)name params:(NSDictionary<NSString*, id> * _Nonnull)params;
- (void) eventRevenue:(MAAd * _Nonnull) ad;

- (void) setAdFreeze:(BOOL)freeze;

@property (nonatomic, strong) NSString * _Nonnull m_adUnitId;
@property (nonatomic, strong) id<AppleAdvertisementInterface> _Nonnull m_advertisement;
@property (nonatomic, strong) MAAdFormat * _Nonnull m_adFormat;

@property (nonatomic, assign) NSInteger m_enumeratorRequest;
@property (nonatomic, assign) NSInteger m_requestId;
@property (nonatomic, assign) NSInteger m_requestAttempt;
@property (nonatomic, assign) NSTimeInterval m_requestTimestamp;

@end
