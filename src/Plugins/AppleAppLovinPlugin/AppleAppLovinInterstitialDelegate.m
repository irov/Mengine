#import "AppleAppLovinInterstitialDelegate.h"

@implementation AppleAppLovinInterstitialDelegate

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString* _Nonnull) adUnitIdentifier{
    self = [super init];
    
    self.m_interstitialAd = [[MAInterstitialAd alloc] initWithAdUnitIdentifier: adUnitIdentifier];
    self.m_interstitialAd.delegate = self;
    
    // Load the first ad
    [self.m_interstitialAd loadAd];
    
    return self;
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

- (void)didLoadAd:(MAAd *)ad
{
    // Interstitial ad is ready to be shown. '[self.interstitialAd isReady]' will now return 'YES'

    // Reset retry attempt
    self.m_retryAttempt = 0;
}

- (void)didFailToLoadAdForAdUnitIdentifier:(NSString *)adUnitIdentifier withError:(MAError *)error
{
    // Interstitial ad failed to load
    // We recommend retrying with exponentially higher delays up to a maximum delay (in this case 64 seconds)
    
    self.m_retryAttempt++;
    NSInteger delaySec = pow(2, MIN(6, self.m_retryAttempt));
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, delaySec * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self.m_interstitialAd loadAd];
    });
}

- (void)didDisplayAd:(MAAd *)ad {}

- (void)didClickAd:(MAAd *)ad {}

- (void)didHideAd:(MAAd *)ad
{
    // Interstitial ad is hidden. Pre-load the next ad
    [self.m_interstitialAd loadAd];
}

- (void)didFailToDisplayAd:(MAAd *)ad withError:(MAError *)error
{
    // Interstitial ad failed to display. We recommend loading the next ad
    [self.m_interstitialAd loadAd];
}

@end
