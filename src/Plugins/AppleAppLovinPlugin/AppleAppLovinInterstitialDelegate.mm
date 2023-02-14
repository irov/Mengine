#import "AppleAppLovinInterstitialDelegate.h"

#include "Kernel/Logger.h"

@implementation AppleAppLovinInterstitialDelegate

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitIdentifier
                                      amazonSlotId:(NSString * _Nullable) amazonSlotId
                                  analyticsService:(AppleAppLovinAnalyticsService * _Nonnull) analytics {
    self = [super init];
    
    self.m_analytics = analytics;
    
    self.m_interstitialAd = [[MAInterstitialAd alloc] initWithAdUnitIdentifier: adUnitIdentifier];
    self.m_interstitialAd.delegate = self;
    self.m_interstitialAd.revenueDelegate = self;
    
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
    if( [amazonSlotId length] != 0 ) {
        self.m_amazonLoader = [[AppleAppLovinInterstitialAmazonLoader alloc] initWithSlotId: amazonSlotId interstitialAd: self.m_interstitialAd];
    }else{
        [self.m_interstitialAd loadAd];
    }
#else
    [self.m_interstitialAd loadAd];
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
    
    self.m_interstitialAd = nil;
    
    [super dealloc];
}

- (BOOL) hasLoaded {
    return [self.m_interstitialAd isReady];
}

- (BOOL) show {
    if( self.hasLoaded == true ) {
        [self.m_interstitialAd showAd];

        return YES;
    }

    return NO;
}

#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *) ad {
    // Interstitial ad is ready to be shown. '[self.interstitialAd isReady]' will now return 'YES'

    // Reset retry attempt
    self.m_retryAttempt = 0;
    
    LOGGER_INFO( "applovin", "Interstitial didLoadAd" );
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *) adUnitIdentifier withError:(MAError *) error {
    // Interstitial ad failed to load
    // We recommend retrying with exponentially higher delays up to a maximum delay (in this case 64 seconds)
    LOGGER_INFO("applovin", "Interstitial didFailToLoadAdForAdUnitIdentifier");
    
    self.m_retryAttempt++;
    NSInteger delaySec = pow(2, MIN(6, self.m_retryAttempt));
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, delaySec * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self.m_interstitialAd loadAd];
    });
}

- (void) didDisplayAd:(MAAd *) ad {
    LOGGER_INFO( "applovin", "Interstitial didDisplayAd" );
}

- (void) didClickAd:(MAAd *) ad {
    LOGGER_INFO( "applovin", "Interstitial didClickAd" );
}

- (void) didHideAd:(MAAd *) ad {
    // Interstitial ad is hidden. Pre-load the next ad
    LOGGER_INFO( "applovin", "Interstitial didHideAd" );
    
    [self.m_interstitialAd loadAd];
}

- (void) didFailToDisplayAd:(MAAd *) ad withError:(MAError *) error {
    LOGGER_INFO( "applovin", "Interstitial didFailToDisplayAd" );
    
    // Interstitial ad failed to display. We recommend loading the next ad
    [self.m_interstitialAd loadAd];
}

#pragma mark - Revenue Callbacks

- (void)didPayRevenueForAd:(MAAd *)ad {
    LOGGER_INFO( "applovin", "Banner didPayRevenueForAd" );
}

@end
