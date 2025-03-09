#import "AppleAppLovinRewardedDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleString.h"

#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSNetwork.h"

#include "AppleAppLovinApplicationDelegate.h"

@implementation AppleAppLovinRewardedDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull)adUnitId {
    self = [super initWithAdUnitIdentifier:adUnitId adFormat:MAAdFormat.rewarded];
    
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
    rewardedAd.adReviewDelegate = self;
    
    self.m_rewardedAd = rewardedAd;
    
    self.m_requestAttempt = 0;
    self.m_enumeratorRequest = 0;
    self.m_requestId = 0;
    
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
    
    self.m_rewardedAd = nil;
}

- (BOOL) canOffer:(NSString * _Nonnull)placement {
    if (self.m_rewardedAd == nil) {
        return NO;
    }
    
    BOOL ready = [self.m_rewardedAd isReady];
    
    [self log:@"canOffer" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    [self event:@"mng_ad_rewarded_offer" params:@{
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
        [self event:@"mng_ad_rewarded_show" params:@{
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
    
    [self event:@"mng_ad_rewarded_show" params:@{
        @"placement": placement,
        @"ready": @(ready)
    }];
    
    if (ready == NO) {
        return NO;
    }
    
    [self.m_rewardedAd showAdForPlacement:placement];

    return YES;
}

- (void) loadAd {
    if (self.m_rewardedAd == nil) {
        return;
    }
    
    [self increaseRequestId];
    
    [self log:@"loadAd"];
    
    [self event:@"mng_ad_rewarded_load" params:@{}];
    
    [self.m_rewardedAd loadAd];
}

#pragma mark - MAAdRequestDelegate Protocol

- (void)didStartAdRequestForAdUnitIdentifier:(NSString *)adUnitIdentifier {
    [self log:@"didStartAdRequestForAdUnitIdentifier"];
    
    [self event:@"mng_ad_rewarded_request_started" params:@{}];
}


#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *)ad {
    [self log:@"didLoadAd" withMAAd:ad];
    
    [self event:@"mng_ad_interstitial_loaded" params:@{
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_requestAttempt = 0;
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *)adUnitIdentifier withError:(MAError *)error {
    [self log:@"didFailToLoadAdForAdUnitIdentifier" withMAError:error];
    
    [self event:@"mng_ad_rewarded_load_failed" params:@{
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code)
    }];
    
    [self retryLoadAd];
}

- (void) didDisplayAd:(MAAd *)ad {
    [self log:@"didDisplayAd" withMAAd:ad];
    
    [self event:@"mng_ad_rewarded_displayed" params:@{
        @"placement": ad.placement,
        @"ad": [self getMAAdParams:ad]
    }];
}

- (void) didClickAd:(MAAd *)ad {
    [self log:@"didClickAd" withMAAd:ad];
    
    [self event:@"mng_ad_rewarded_clicked" params:@{
        @"placement": ad.placement,
        @"ad": [self getMAAdParams:ad]
    }];
}

- (void) didHideAd:(MAAd *)ad {
    [self log:@"didHideAd" withMAAd:ad];
    
    [self event:@"mng_ad_rewarded_hidden" params:@{
        @"placement": ad.placement,
        @"ad": [self getMAAdParams:ad]
    }];

    id<AppleAdvertisementResponseInterface> response = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementResponse];
    
    if (response != nil) {
        [response onAppleAdvertisementShowSuccess:[iOSAdFormat ADFORMAT_REWARDED] withPlacement:ad.placement];
    }

    [self loadAd];
}

- (void) didFailToDisplayAd:(MAAd *)ad withError:(MAError *)error {
    [self log:@"didFailToDisplayAd" withMAAd:ad withMAError:error];
    
    [self event:@"mng_ad_rewarded_display_failed" params:@{
        @"placement": ad.placement,
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code),
        @"ad": [self getMAAdParams:ad]
    }];

    id<AppleAdvertisementResponseInterface> response = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementResponse];
    
    if (response != nil) {
        [response onAppleAdvertisementShowFailed:[iOSAdFormat ADFORMAT_REWARDED] withPlacement:ad.placement withError:error.code];
    }
    
    [self loadAd];
}

#pragma mark - MARewardedAdDelegate Protocol

- (void) didStartRewardedVideoForAd:(MAAd *)ad {
    [self log:@"didStartRewardedVideoForAd" withMAAd:ad];
}

- (void) didCompleteRewardedVideoForAd:(MAAd *)ad {
    [self log:@"didCompleteRewardedVideoForAd" withMAAd:ad];
}

- (void) didRewardUserForAd:(MAAd *)ad withReward:(MAReward *)reward {
    [self log:@"didRewardUserForAd" withMAAd:ad withMAReward:reward];
    
    [self event:@"mng_ad_rewarded_reward_user" params:@{
        @"placement": ad.placement,
        @"label": reward.label,
        @"amount": @(reward.amount),
        @"ad": [self getMAAdParams:ad]
    }];
    
    id<AppleAdvertisementResponseInterface> response = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementResponse];
    
    if (response != nil) {
        [response onAppleAdvertisementUserRewarded:[iOSAdFormat ADFORMAT_REWARDED] withPlacement:ad.placement withLabel:reward.label withAmount:reward.amount];
    }
}

#pragma mark - Revenue Callbacks

- (void)didPayRevenueForAd:(MAAd *)ad {
    [self log:@"didPayRevenueForAd" withMAAd:ad];
    
    [self event:@"mng_ad_rewarded_revenue_paid" params:@{
        @"placement": ad.placement,
        @"revenue_value": @(ad.revenue),
        @"revenue_precision": ad.revenuePrecision,
        @"ad": [self getMAAdParams:ad]
    }];
    
    [self eventRevenue:ad];

    id<AppleAdvertisementResponseInterface> response = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementResponse];
    
    if (response != nil) {
        [response onAppleAdvertisementRevenuePaid:[iOSAdFormat ADFORMAT_REWARDED] withPlacement:ad.placement withRevenue:ad.revenue];
    }
}

#pragma mark - AdReview Callbacks

- (void)didGenerateCreativeIdentifier:(NSString *)creativeIdentifier forAd:(MAAd *)ad {
    //ToDo
}
    
@end
