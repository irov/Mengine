#import "AppleAppLovinRewardedDelegate.h"

#include "Interface/PlatformServiceInterface.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleString.h"

#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSNetwork.h"

#import "AppleAppLovinApplicationDelegate.h"

@implementation AppleAppLovinRewardedDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull)adUnitId advertisement:(id<AppleAdvertisementInterface> _Nonnull)advertisement {
    self = [super initWithAdUnitIdentifier:adUnitId adFormat:MAAdFormat.rewarded advertisement:advertisement];
    
    MARewardedAd * rewardedAd;
    
    @try {
        rewardedAd = [MARewardedAd sharedWithAdUnitIdentifier:adUnitId];
    } @catch (NSException * ex) {
        IOS_LOGGER_MESSAGE(@"[Error] AppleAppLovinRewardedDelegate invalid create MARewardedAd adUnitId: %@ exception: %@ [%@]"
            , adUnitId
            , ex.reason
            , ex.name
        );
        
        return nil;
    }
    
    rewardedAd.delegate = self;
    rewardedAd.requestDelegate = self;
    rewardedAd.revenueDelegate = self;
    rewardedAd.expirationDelegate = self;
    rewardedAd.adReviewDelegate = self;
    
    self.m_rewardedAd = rewardedAd;
    
    self.m_showing = NO;
    
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
    self.m_amazonLoader = [[AppleAppLovinRewardedAmazonLoader alloc] initWithSlotId:amazonSlotId rewardedAd:self.m_rewardedAd];
#else
    [self loadAd];
#endif
    
    return self;
}

- (void) dealloc {
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
    if (self.m_amazonLoader != nil) {
        [self.m_amazonLoader release];
        self.m_amazonLoader = nil;
    }
#endif
    
    if (self.m_rewardedAd != nil) {
        self.m_rewardedAd.delegate = nil;
        self.m_rewardedAd.requestDelegate = nil;
        self.m_rewardedAd.revenueDelegate = nil;
        self.m_rewardedAd.expirationDelegate = nil;
        self.m_rewardedAd.adReviewDelegate = nil;
        
        self.m_rewardedAd = nil;
    }
}

- (void) eventRewarded:(NSString * _Nonnull) eventName params:(NSDictionary<NSString *, id> * _Nullable) params {
    [self event:[@"mng_applovin_rewarded_" stringByAppendingString:eventName] params:params];
}

- (BOOL) canOffer:(NSString * _Nonnull)placement {
    if (self.m_rewardedAd == nil) {
        return NO;
    }
    
    BOOL ready = [self.m_rewardedAd isReady];
    
    [self log:@"canOffer" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    [self eventRewarded:@"offer" params:@{
        @"placement": placement,
        @"ready": @(ready)
    }];
    
    return ready;
}

- (BOOL) canYouShow:(NSString * _Nonnull)placement {
    if (self.m_rewardedAd == nil) {
        return NO;
    }
    
    BOOL ready = [self.m_rewardedAd isReady];

    [self log:@"canYouShow" withParams:@{@"placement":placement, @"ready":@(ready)}];

    if( ready == NO ) {
        [self eventRewarded:@"show" params:@{
            @"placement": placement,
            @"ready": @(NO)
        }];
        
        return NO;
    }
    
    return YES;
}

- (BOOL) show:(NSString * _Nonnull)placement {
    if (self.m_rewardedAd == nil) {
        return NO;
    }
    
    BOOL ready = [self.m_rewardedAd isReady];
       
    [self log:@"show" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    [self eventRewarded:@"show" params:@{
        @"placement": placement,
        @"ready": @(ready)
    }];
    
    if (ready == NO) {
        return NO;
    }
    
    self.m_showing = YES;
    
    [self.m_rewardedAd showAdForPlacement:placement];

    return YES;
}

- (BOOL) isShowing {
    return self.m_showing;
}

- (void) loadAd {
    if (self.m_rewardedAd == nil) {
        return;
    }
    
    [self increaseRequestId];
    
    [self log:@"loadAd"];
    
    [self eventRewarded:@"load" params:@{}];
    
    [self.m_rewardedAd loadAd];
}

#pragma mark - MAAdRequestDelegate

- (void)didStartAdRequestForAdUnitIdentifier:(NSString *)adUnitIdentifier {
    [self log:@"didStartAdRequestForAdUnitIdentifier"];
    
    [self eventRewarded:@"request_started" params:@{}];
}


#pragma mark - MAAdDelegate

- (void) didLoadAd:(MAAd *)ad {
    [self log:@"didLoadAd" withMAAd:ad];
    
    [self eventRewarded:@"loaded" params:@{
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_requestAttempt = 0;
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *)adUnitIdentifier withError:(MAError *)error {
    [self log:@"didFailToLoadAdForAdUnitIdentifier" withMAError:error];
    
    [self eventRewarded:@"load_failed" params:@{
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code)
    }];
    
    [self retryLoadAd];
}

- (void) didDisplayAd:(MAAd *)ad {
    [self log:@"didDisplayAd" withMAAd:ad];
    
    [self eventRewarded:@"displayed" params:@{
        @"placement": ad.placement,
        @"ad": [self getMAAdParams:ad]
    }];
    
    PLATFORM_SERVICE()
        ->freezePlatform( true, true, true );
}

- (void) didClickAd:(MAAd *)ad {
    [self log:@"didClickAd" withMAAd:ad];
    
    [self eventRewarded:@"clicked" params:@{
        @"placement": ad.placement,
        @"ad": [self getMAAdParams:ad]
    }];
}

- (void) didHideAd:(MAAd *)ad {
    [self log:@"didHideAd" withMAAd:ad];
    
    [self eventRewarded:@"hidden" params:@{
        @"placement": ad.placement,
        @"ad": [self getMAAdParams:ad]
    }];
    
    PLATFORM_SERVICE()
        ->unfreezePlatform( true, true, true );
    
    self.m_showing = NO;

    id<AppleAdvertisementCallbackInterface> callback = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementRewardedCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementShowSuccess:ad.placement];
    }

    [self loadAd];
}

- (void) didFailToDisplayAd:(MAAd *)ad withError:(MAError *)error {
    [self log:@"didFailToDisplayAd" withMAAd:ad withMAError:error];
    
    [self eventRewarded:@"display_failed" params:@{
        @"placement": ad.placement,
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_showing = NO;

    id<AppleAdvertisementCallbackInterface> callback = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementRewardedCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementShowFailed:ad.placement withError:error.code];
    }
    
    [self loadAd];
}

#pragma mark - MARewardedAdDelegate

- (void) didStartRewardedVideoForAd:(MAAd *)ad {
    [self log:@"didStartRewardedVideoForAd" withMAAd:ad];
}

- (void) didCompleteRewardedVideoForAd:(MAAd *)ad {
    [self log:@"didCompleteRewardedVideoForAd" withMAAd:ad];
}

- (void) didRewardUserForAd:(MAAd *)ad withReward:(MAReward *)reward {
    [self log:@"didRewardUserForAd" withMAAd:ad withMAReward:reward];
    
    [self eventRewarded:@"reward_user" params:@{
        @"placement": ad.placement,
        @"label": reward.label,
        @"amount": @(reward.amount),
        @"ad": [self getMAAdParams:ad]
    }];
    
    id<AppleAdvertisementCallbackInterface> callback = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementRewardedCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementUserRewarded:ad.placement withLabel:reward.label withAmount:reward.amount];
    }
}

#pragma mark - MAAdRevenueDelegate

- (void)didPayRevenueForAd:(MAAd *)ad {
    [self log:@"didPayRevenueForAd" withMAAd:ad];

    [self eventRevenue:ad];

    id<AppleAdvertisementCallbackInterface> callback = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementRewardedCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementRevenuePaid:ad.placement withRevenue:ad.revenue];
    }
}

#pragma mark - MAAdExpirationDelegate

- (void)didReloadExpiredAd:(MAAd *)expiredAd withNewAd:(MAAd *)newAd {
    [self log:@"didReloadExpiredAd" withMAAd:expiredAd];
    
    [self eventRewarded:@"expired" params:@{
        @"placement": expiredAd.placement,
        @"ad": [self getMAAdParams:expiredAd],
        @"new_ad": [self getMAAdParams:newAd]
    }];
}

#pragma mark - MAAdReviewDelegate

- (void)didGenerateCreativeIdentifier:(NSString *)creativeIdentifier forAd:(MAAd *)ad {
    //ToDo
}
    
@end
