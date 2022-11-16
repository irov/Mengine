#import "AppleAppLovinRewardedDelegate.h"

#include "Kernel/Logger.h"

@implementation AppleAppLovinRewardedDelegate

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString* _Nonnull) adUnitIdentifier
                                      amazonSlotId:(NSString* _Nullable) amazonSlotId
                                  analyticsService:(AppleAppLovinAnalyticsService * _Nonnull) analytics {
    self = [super init];
    
    self.m_analytics = analytics;
    
    self.m_rewardedAd = [MARewardedAd sharedWithAdUnitIdentifier: adUnitIdentifier];
    self.m_rewardedAd.delegate = self;
    self.m_rewardedAd.revenueDelegate = self;
    
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
    if( [amazonSlotId length] != 0 ) {
        self.m_amazonLoader = [[AppleAppLovinRewardedAmazonLoader alloc] initWithSlotId: amazonSlotId rewardedAd: self.m_rewardedAd];
    }else{
        // Load the first ad
        [self.m_rewardedAd loadAd];
    }
#else
    [self.m_rewardedAd loadAd];
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
    
    [super dealloc];
}

- (BOOL) hasLoaded {
    return [self.m_rewardedAd isReady];
}

- (BOOL) show {
    if( self.hasLoaded == true ) {
        [self.m_rewardedAd showAd];

        return YES;
    }

    return NO;
}

#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *) ad {
    LOGGER_INFO("applovin", "Rewarded didLoadAd");
    
    // Reset retry attempt
    self.m_retryAttempt = 0;
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *) adUnitIdentifier withError:(MAError *) error {
    LOGGER_INFO("applovin", "Rewarded didFailToLoadAdForAdUnitIdentifier");
    
    self.m_retryAttempt++;

    NSInteger delaySec = pow(2, MIN(6, self.m_retryAttempt));
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, delaySec * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self.m_rewardedAd loadAd];
    });
}

- (void) didDisplayAd:(MAAd *) ad {
    LOGGER_INFO("applovin", "Rewarded didDisplayAd");
}

- (void) didClickAd:(MAAd *) ad {
    LOGGER_INFO("applovin", "Rewarded didClickAd");
}

- (void) didHideAd:(MAAd *) ad {
    LOGGER_INFO("applovin", "Rewarded didHideAd");
    
    // Rewarded ad is hidden. Pre-load the next ad
    [self.m_rewardedAd loadAd];
}

- (void) didFailToDisplayAd:(MAAd *) ad withError:(MAError *) error {
    LOGGER_INFO("applovin", "Rewarded didFailToDisplayAd");
        
    [self.m_rewardedAd loadAd];
}

#pragma mark - MARewardedAdDelegate Protocol

- (void) didStartRewardedVideoForAd:(MAAd *)ad {
    LOGGER_INFO("applovin", "Rewarded didStartRewardedVideoForAd");
}

- (void) didCompleteRewardedVideoForAd:(MAAd *)ad {
    LOGGER_INFO("applovin", "Rewarded didCompleteRewardedVideoForAd");
}

- (void) didRewardUserForAd:(MAAd *) ad withReward:(MAReward *) reward {
    LOGGER_INFO("applovin", "Rewarded didRewardUserForAd");
}

#pragma mark - Revenue Callbacks

- (void)didPayRevenueForAd:(MAAd *)ad {
    LOGGER_INFO("applovin", "Banner didPayRevenueForAd" );
        
    [self.m_analytics eventRevenuePaid:ad];
}
    
@end
