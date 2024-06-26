#pragma once

#import "AppleAppLovinInterface.h"

#import <AppLovinSDK/AppLovinSDK.h>
#import <Foundation/Foundation.h>

@interface AppleAppLovinBaseDelegate : NSObject

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                          adFormat:(MAAdFormat * _Nonnull) adFormat;

- (void) loadAd;
- (void) retryLoadAd;

- (NSDictionary * _Nonnull) getMAAdParams:(MAAd * _Nonnull) ad;
- (NSDictionary * _Nonnull) getMAErrorParams:(MAError * _Nonnull) error;
- (NSDictionary * _Nonnull) getMARewardParams:(MAReward * _Nonnull) reward;

- (void) log:(NSString * _Nonnull) callback;
- (void) log:(NSString * _Nonnull) callback withParams:(NSDictionary * _Nonnull) params;
- (void) log:(NSString * _Nonnull) callback withMAAd:(MAAd * _Nonnull) ad;
- (void) log:(NSString * _Nonnull) callback withMAAd:(MAAd * _Nonnull) ad withMAError:(MAError * _Nonnull)error;
- (void) log:(NSString * _Nonnull) callback withMAError:(MAError * _Nonnull)error;
- (void) log:(NSString * _Nonnull) callback withMAAd:(MAAd * _Nonnull) ad withMAReward:(MAReward * _Nonnull)reward;

- (void) eventRevenue:(MAAd * _Nonnull) ad;

@property (strong) NSString * _Nonnull m_adUnitId;
@property (strong) MAAdFormat * _Nonnull m_adFormat;

@property (nonatomic, assign) NSInteger m_retryAttempt;
@property (nonatomic, assign) NSInteger m_enumeratorRequest;
@property (nonatomic, assign) NSInteger m_requestId;

@end
