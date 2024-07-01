#import "AppleAppLovinRewardedDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleString.h"
#import "Environment/Apple/AppleAnalytics.h"

#include "AppleAppLovinApplicationDelegate.h"

#include "Kernel/Logger.h"

@implementation AppleAppLovinRewardedDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                       amazonSlotId:(NSString * _Nullable) amazonSlotId
                                           provider:(const Mengine::AppleAppLovinRewardedProviderInterfacePtr &) provider {
    self = [super initWithAdUnitIdentifier:adUnitId adFormat:MAAdFormat.rewarded];

    self.m_provider = provider;
    
    MARewardedAd * rewardedAd;
    
    @try {
        rewardedAd = [MARewardedAd sharedWithAdUnitIdentifier:adUnitId];
    } @catch (NSException * ex) {
        LOGGER_ERROR( "[Error] AppleAppLovinRewardedDelegate invalid create MARewardedAd adUnitId: %s exception: %s [%s]"
            , [adUnitId UTF8String]
            , [ex.reason UTF8String]
            , [ex.name UTF8String]
        );
        
        return nil;
    }
    
    rewardedAd.delegate = self;
    rewardedAd.requestDelegate = self;
    rewardedAd.revenueDelegate = self;
    rewardedAd.adReviewDelegate = self;
    
    self.m_rewardedAd = rewardedAd;
    
    self.m_retryAttempt = 0;
    self.m_enumeratorRequest = 0;
    self.m_requestId = 0;
    
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
    self.m_amazonLoader = [[AppleAppLovinRewardedAmazonLoader alloc] initWithSlotId:amazonSlotId rewardedAd:self.m_rewardedAd];
#else
    [self loadAd];
#endif
    
    return self;
}

- (void) dealloc {
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
    if( self.m_amazonLoader != nil ) {
        [self.m_amazonLoader release];
        self.m_amazonLoader = nil;
    }
#endif
    
    self.m_rewardedAd = nil;
}

- (BOOL) canOffer:(NSString * _Nonnull) placement {
    BOOL ready = [self.m_rewardedAd isReady];
    
    [self log:@"canOffer" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    [AppleAnalytics event:@"mng_ad_rewarded_offer" params:@{
        @"ad_unit_id": self.m_adUnitId,
        @"placement": placement,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"ready": @(ready)
    }];
    
    return ready;
}

- (BOOL) canYouShow:(NSString * _Nonnull) placement {
    BOOL ready = [self.m_rewardedAd isReady];

    [self log:@"canYouShow" withParams:@{@"placement":placement, @"ready":@(ready)}];

    if( ready == NO ) {
        [AppleAnalytics event:@"mng_ad_rewarded_show" params:@{
            @"ad_unit_id": self.m_adUnitId,
            @"placement": placement,
            @"request_id": @(self.m_requestId),
            @"attempt": @(self.m_retryAttempt),
            @"ready": @(NO)
        }];
        
        return NO;
    }
    
    return YES;
}

- (BOOL) show:(NSString * _Nonnull) placement {
    BOOL ready = [self.m_rewardedAd isReady];
       
    [self log:@"show" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    [AppleAnalytics event:@"mng_ad_rewarded_show" params:@{
        @"ad_unit_id": self.m_adUnitId,
        @"placement": placement,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"ready": @(ready)
    }];
    
    if( ready == NO ) {
        return NO;
    }
    
    [self.m_rewardedAd showAdForPlacement:placement];

    return YES;
}

- (void) loadAd {
    if( self.m_rewardedAd == nil ) {
        return;
    }
    
    self.m_requestId = self.m_enumeratorRequest++;
    
    [self log:@"loadAd"];
    
    [AppleAnalytics event:@"mng_ad_rewarded_load" params:@{
        @"ad_unit_id": self.m_adUnitId,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt)
    }];
    
    [self.m_rewardedAd loadAd];
}

#pragma mark - MAAdRequestDelegate Protocol

- (void)didStartAdRequestForAdUnitIdentifier:(NSString *)adUnitIdentifier {
    [self log:@"didStartAdRequestForAdUnitIdentifier"];
    
    [AppleAnalytics event:@"mng_ad_rewarded_request_started" params:@{
        @"ad_unit_id": adUnitIdentifier,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt)
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidStartAdRequestForAdUnitIdentifier();
}


#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *) ad {
    [self log:@"didLoadAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_interstitial_loaded" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidLoadAd();
    
    self.m_retryAttempt = 0;
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *) adUnitIdentifier withError:(MAError *) error {
    [self log:@"didFailToLoadAdForAdUnitIdentifier" withMAError:error];
    
    [AppleAnalytics event:@"mng_ad_rewarded_load_failed" params:@{
        @"ad_unit_id": adUnitIdentifier,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code)
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidFailToLoadAdForAdUnitIdentifier();
    
    [self retryLoadAd];
}

- (void) didDisplayAd:(MAAd *) ad {
    [self log:@"didDisplayAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_rewarded_displayed" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidDisplayAd();
}

- (void) didClickAd:(MAAd *) ad {
    [self log:@"didClickAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_rewarded_clicked" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidClickAd();
}

- (void) didHideAd:(MAAd *) ad {
    [self log:@"didHideAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_rewarded_hidden" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidHideAd();
    
    [self loadAd];
}

- (void) didFailToDisplayAd:(MAAd *) ad withError:(MAError *) error {
    [self log:@"didFailToDisplayAd" withMAAd:ad withMAError:error];
    
    [AppleAnalytics event:@"mng_ad_rewarded_display_failed" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidFailToDisplayAd();
    
    [self loadAd];
}

#pragma mark - MARewardedAdDelegate Protocol

- (void) didStartRewardedVideoForAd:(MAAd *) ad {
    [self log:@"didStartRewardedVideoForAd" withMAAd:ad];
    
    self.m_provider->onAppleAppLovinRewardedDidStartRewardedVideoForAd();
}

- (void) didCompleteRewardedVideoForAd:(MAAd *) ad {
    [self log:@"didCompleteRewardedVideoForAd" withMAAd:ad];
    
    self.m_provider->onAppleAppLovinRewardedDidCompleteRewardedVideoForAd();
}

- (void) didRewardUserForAd:(MAAd *) ad withReward:(MAReward *) reward {
    [self log:@"didRewardUserForAd" withMAAd:ad withMAReward:reward];
    
    [AppleAnalytics event:@"mng_ad_rewarded_reward_user" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad],
        @"label": reward.label,
        @"amount": @(reward.amount)
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidRewardUserForAd();
}

#pragma mark - Revenue Callbacks

- (void)didPayRevenueForAd:(MAAd *) ad {
    [self log:@"didPayRevenueForAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_rewarded_revenue_paid" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"revenue_value": @(ad.revenue),
        @"revenue_precision": ad.revenuePrecision,
        @"ad": [self getMAAdParams:ad]
    }];
    
    [self eventRevenue:ad];
    
    Mengine::Params params;
    Mengine::Helper::AppleGetParamsFromNSDictionary( @{@"revenue":@(ad.revenue)}, &params );
    
    self.m_provider->onAppleAppLovinRewardedDidPayRevenueForAd( params );
}

#pragma mark - AdReview Callbacks

- (void)didGenerateCreativeIdentifier:(NSString *)creativeIdentifier forAd:(MAAd *)ad {
    //ToDo
}
    
@end
