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
    
    self.m_retryAttempt = 0;
    self.m_enumeratorRequest = 0;
    self.m_requestId = 0;
    
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
    self.m_amazonLoader = [[AppleAppLovinRewardedAmazonLoader alloc] initWithSlotId: amazonSlotId rewardedAd: self.m_rewardedAd];
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
    
    return ready;
}

- (BOOL) canYouShow {
    BOOL ready = [self.m_rewardedAd isReady];
    
    return ready;
}

- (BOOL) show {
    BOOL ready = [self.m_rewardedAd isReady];
    
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
    
    [self.m_rewardedAd loadAd];
}

#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *) ad {
    LOGGER_MESSAGE( "rewarded didLoadAd" );
    
    self.m_retryAttempt = 0;
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *) adUnitIdentifier withError:(MAError *) error {
    LOGGER_MESSAGE( "rewarded didFailToLoadAdForAdUnitIdentifier" );
    
    self.m_retryAttempt++;

    NSInteger delaySec = pow(2, MIN(6, self.m_retryAttempt));
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, delaySec * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self loadAd];
    });
}

- (void) didDisplayAd:(MAAd *) ad {
    LOGGER_MESSAGE( "rewarded didDisplayAd" );
}

- (void) didClickAd:(MAAd *) ad {
    LOGGER_MESSAGE( "rewarded didClickAd" );
}

- (void) didHideAd:(MAAd *) ad {
    LOGGER_MESSAGE( "rewarded didHideAd" );
    
    // Rewarded ad is hidden. Pre-load the next ad
    [self.m_rewardedAd loadAd];
}

- (void) didFailToDisplayAd:(MAAd *) ad withError:(MAError *) error {
    LOGGER_MESSAGE( "rewarded didFailToDisplayAd" );
        
    [self.m_rewardedAd loadAd];
}

#pragma mark - MARewardedAdDelegate Protocol

- (void) didStartRewardedVideoForAd:(MAAd *)ad {
    LOGGER_MESSAGE( "rewarded didStartRewardedVideoForAd" );
}

- (void) didCompleteRewardedVideoForAd:(MAAd *)ad {
    LOGGER_MESSAGE( "rewarded didCompleteRewardedVideoForAd" );
}

- (void) didRewardUserForAd:(MAAd *) ad withReward:(MAReward *) reward {
    LOGGER_MESSAGE( "rewarded didRewardUserForAd" );
}

#pragma mark - Revenue Callbacks

- (void)didPayRevenueForAd:(MAAd *)ad {
    LOGGER_MESSAGE( "rewarded didPayRevenueForAd" );
        
    [self.m_analytics eventRevenuePaid:ad];
}
    
@end
