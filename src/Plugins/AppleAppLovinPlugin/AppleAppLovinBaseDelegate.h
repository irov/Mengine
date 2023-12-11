#pragma once

#import "AppleAppLovinAnalyticsService.h"
#import "AppleAppLovinInterface.h"

#import <AppLovinSDK/AppLovinSDK.h>
#import <Foundation/Foundation.h>

@interface AppleAppLovinBaseDelegate : NSObject

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                          adFormat:(MAAdFormat * _Nonnull) adFormat
                                         analytics:(AppleAppLovinAnalyticsService * _Nonnull) analytics;

- (void) loadAd;
- (void) retryLoadAd;

- (NSString * _Nonnull) getMAAdParams:(MAAd * _Nonnull) ad;
- (NSString * _Nonnull) getMAErrorParams:(MAError * _Nonnull) error;
- (NSString * _Nonnull) getMARewardParams:(MAReward * _Nonnull) reward;

- (void) log:(NSString * _Nonnull) callback;
- (void) log:(NSString * _Nonnull) callback withParams:(NSDictionary * _Nonnull) params;
- (void) log:(NSString * _Nonnull) callback withMAAd:(MAAd * _Nonnull) ad;
- (void) log:(NSString * _Nonnull) callback withMAAd:(MAAd * _Nonnull) ad withMAError:(MAError * _Nonnull)error;
- (void) log:(NSString * _Nonnull) callback withMAError:(MAError * _Nonnull)error;
- (void) log:(NSString * _Nonnull) callback withMAAd:(MAAd * _Nonnull) ad withMAReward:(MAReward * _Nonnull)reward;

@property (strong) NSString * _Nonnull m_adUnitId;
@property (strong) MAAdFormat * _Nonnull m_adFormat;

@property (nonatomic, strong) AppleAppLovinAnalyticsService * _Nonnull m_analytics;

@property (nonatomic, assign) NSInteger m_retryAttempt;
@property (nonatomic, assign) NSInteger m_enumeratorRequest;
@property (nonatomic, assign) NSInteger m_requestId;

@end
