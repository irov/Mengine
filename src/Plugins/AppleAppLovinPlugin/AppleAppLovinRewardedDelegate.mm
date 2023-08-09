#import "AppleAppLovinRewardedDelegate.h"

#include "Environment/Apple/AppleString.h"
#include "Environment/Apple/AppleAnalytics.h"

#include "Kernel/Logger.h"

@implementation AppleAppLovinRewardedDelegate

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                      amazonSlotId:(NSString * _Nullable) amazonSlotId
                                          provider:(const Mengine::AppleAppLovinRewardedProviderInterfacePtr &) provider
                                         analytics:(AppleAppLovinAnalyticsService * _Nonnull) analytics {
    self = [super initWithAdUnitIdentifier:adUnitId analytics:analytics];
    
    self.m_rewardedAd = [MARewardedAd sharedWithAdUnitIdentifier: adUnitId];
    self.m_rewardedAd.delegate = self;
    self.m_rewardedAd.requestDelegate = self;
    self.m_rewardedAd.revenueDelegate = self;
    
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
    if( self.m_amazonLoader != nil )
    {
        [self.m_amazonLoader release];
        self.m_amazonLoader = nil;
    }
#endif
    
    self.m_rewardedAd = nil;
    
    [super dealloc];
}

- (BOOL) canOffer {
    BOOL ready = [self.m_rewardedAd isReady];
    
    LOGGER_MESSAGE("canOffer rewarded ready: %d request: %ld attempt: %ld"
        , ready
        , self.m_requestId
        , self.m_retryAttempt
        );
    
    [AppleAnalytics event:@"mng_ad_rewarded_offer" params:@{
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"ready": @(ready)
    }];
    
    return ready;
}

- (BOOL) canYouShow {
    BOOL ready = [self.m_rewardedAd isReady];

    LOGGER_MESSAGE("canYouShow rewarded ready: %d request: %ld attempt: %ld"
        , ready
        , self.m_requestId
        , self.m_retryAttempt
    );

    if( ready == NO )
    {
        [AppleAnalytics event:@"mng_ad_rewarded_show" params:@{
            @"request_id": @(self.m_requestId),
            @"attempt": @(self.m_retryAttempt),
            @"ready": @(NO)
        }];
        
        return NO;
    }
    
    return YES;
}

- (BOOL) show {
    BOOL ready = [self.m_rewardedAd isReady];
    
    LOGGER_MESSAGE("show rewarded ready: %d request: %ld attempt: %ld"
        , ready
        , self.m_requestId
        , self.m_retryAttempt
    );
    
    [AppleAnalytics event:@"mng_ad_rewarded_show" params:@{
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"ready": @(ready)
    }];
    
    if( ready == NO )
    {
        return NO;
    }
    
    [self.m_rewardedAd showAd];

    return YES;
}

- (void) loadAd {
    if( self.m_rewardedAd == nil )
    {
        return;
    }
    
    self.m_requestId = self.m_enumeratorRequest++;
    
    LOGGER_MESSAGE( "load rewarded request: %ld attempt: %ld"
        , self.m_requestId
        , self.m_retryAttempt
    );
    
    [AppleAnalytics event:@"mng_ad_rewarded_load" params:@{
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt)
    }];
    
    [self.m_rewardedAd loadAd];
}

#pragma mark - MAAdRequestDelegate Protocol

- (void)didStartAdRequestForAdUnitIdentifier:(NSString *)adUnitIdentifier {
    LOGGER_MESSAGE( "rewarded didStartAdRequestForAdUnitIdentifier: %s request: %ld attempt: %ld"
        , [adUnitIdentifier UTF8String]
        , self.m_requestId
        , self.m_retryAttempt
    );
    
    [AppleAnalytics event:@"mng_ad_rewarded_request_started" params:@{
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"unit_id": adUnitIdentifier
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidStartAdRequestForAdUnitIdentifier();
}


#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *) ad {
    LOGGER_MESSAGE( "rewarded didLoadAd: %s request: %ld attempt: %ld"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
        , self.m_retryAttempt
    );
    
    [AppleAnalytics event:@"mng_ad_interstitial_loaded" params:@{
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidLoadAd();
    
    self.m_retryAttempt = 0;
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *) adUnitIdentifier withError:(MAError *) error {
    LOGGER_MESSAGE( "rewarded didFailToLoadAdForAdUnitIdentifier: %s request: %ld attempt: %ld error: %s"
        , [adUnitIdentifier UTF8String]
        , self.m_requestId
        , self.m_retryAttempt
        , [[self getMAErrorParams:error] UTF8String]
    );
    
    [AppleAnalytics event:@"mng_ad_rewarded_load_failed" params:@{
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"unit_id": adUnitIdentifier,
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code)
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidFailToLoadAdForAdUnitIdentifier();
    
    self.m_retryAttempt++;

    NSInteger delaySec = pow(2, MIN(6, self.m_retryAttempt));
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, delaySec * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self loadAd];
    });
}

- (void) didDisplayAd:(MAAd *) ad {
    LOGGER_MESSAGE( "rewarded didDisplayAd: %s request: %ld"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
    );
    
    [AppleAnalytics event:@"mng_ad_rewarded_displayed" params:@{
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidDisplayAd();
}

- (void) didClickAd:(MAAd *) ad {
    LOGGER_MESSAGE( "rewarded didClickAd: %s request: %ld"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
    );
    
    [AppleAnalytics event:@"mng_ad_rewarded_clicked" params:@{
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidClickAd();
}

- (void) didHideAd:(MAAd *) ad {
    LOGGER_MESSAGE( "rewarded didHideAd: %s request: %ld"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
    );
    
    [AppleAnalytics event:@"mng_ad_rewarded_hidden" params:@{
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidHideAd();
    
    [self loadAd];
}

- (void) didFailToDisplayAd:(MAAd *) ad withError:(MAError *) error {
    LOGGER_MESSAGE( "rewarded didFailToDisplayAd: %s request: %ld error: %s"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
        , [[self getMAErrorParams:error] UTF8String]
    );
    
    [AppleAnalytics event:@"mng_ad_rewarded_display_failed" params:@{
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidFailToDisplayAd();
    
    [self loadAd];
}

#pragma mark - MARewardedAdDelegate Protocol

- (void) didStartRewardedVideoForAd:(MAAd *)ad {
    LOGGER_MESSAGE( "rewarded didStartRewardedVideoForAd: %s request: %ld"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
    );
    
    self.m_provider->onAppleAppLovinRewardedDidStartRewardedVideoForAd();
}

- (void) didCompleteRewardedVideoForAd:(MAAd *)ad {
    LOGGER_MESSAGE( "rewarded didCompleteRewardedVideoForAd: %s request: %ld"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
    );
    
    self.m_provider->onAppleAppLovinRewardedDidCompleteRewardedVideoForAd();
}

- (void) didRewardUserForAd:(MAAd *) ad withReward:(MAReward *) reward {
    LOGGER_MESSAGE( "rewarded didRewardUserForAd: %s reward: %s requst: %ld"
        , [[self getMAAdParams:ad] UTF8String]
        , [[self getMARewardParams:reward] UTF8String]
        , self.m_requestId
    );
    
    [AppleAnalytics event:@"mng_ad_rewarded_reward_user" params:@{
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad],
        @"label": reward.label,
        @"amount": @(reward.amount)
    }];
    
    self.m_provider->onAppleAppLovinRewardedDidRewardUserForAd();
}

#pragma mark - Revenue Callbacks

- (void)didPayRevenueForAd:(MAAd *)ad {
    LOGGER_MESSAGE( "rewarded didPayRevenueForAd: %s request: %ld"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
    );
    
    [AppleAnalytics event:@"mng_ad_rewarded_revenue_paid" params:@{
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
        
    [self.m_analytics eventRevenuePaid:ad];
    
    self.m_provider->onAppleAppLovinRewardedDidPayRevenueForAd();
}
    
@end
