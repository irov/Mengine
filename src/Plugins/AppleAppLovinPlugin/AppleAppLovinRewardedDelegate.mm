#import "AppleAppLovinRewardedDelegate.h"

@implementation AppleAppLovinRewardedDelegate

@synthesize m_callback;

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString* _Nonnull) adUnitIdentifier rewardCallback:(Mengine::AppleAppLovinRewardCallbackInterface * _Nonnull) callback {
    self = [super init];
    
    self.m_callback = callback;
    
    self.m_rewardedAd = [MARewardedAd sharedWithAdUnitIdentifier: adUnitIdentifier];
    self.m_rewardedAd.delegate = self;
    
    // Load the first ad
    [self.m_rewardedAd loadAd];
    
    return self;
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

- (void)didLoadAd:(MAAd *)ad
{
    // Rewarded ad is ready to be shown. '[self.rewardedAd isReady]' will now return 'YES'
    
    // Reset retry attempt
    self.m_retryAttempt = 0;
}

- (void)didFailToLoadAdForAdUnitIdentifier:(NSString *)adUnitIdentifier withError:(MAError *)error
{
    // Rewarded ad failed to load
    // We recommend retrying with exponentially higher delays up to a maximum delay (in this case 64 seconds)
    
    self.m_retryAttempt++;

    NSInteger delaySec = pow(2, MIN(6, self.m_retryAttempt));
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, delaySec * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self.m_rewardedAd loadAd];
    });
}

- (void)didDisplayAd:(MAAd *)ad {}

- (void)didClickAd:(MAAd *)ad {}

- (void)didHideAd:(MAAd *)ad
{
    // Rewarded ad is hidden. Pre-load the next ad
    [self.m_rewardedAd loadAd];
}

- (void)didFailToDisplayAd:(MAAd *)ad withError:(MAError *)error
{
    // Rewarded ad failed to display. We recommend loading the next ad
    [self.m_rewardedAd loadAd];
}

#pragma mark - MARewardedAdDelegate Protocol

- (void)didStartRewardedVideoForAd:(MAAd *)ad {}

- (void)didCompleteRewardedVideoForAd:(MAAd *)ad {}

- (void)didRewardUserForAd:(MAAd *)ad withReward:(MAReward *)reward
{
    m_callback->onAppLovinRewardReceivedReward( reward.amount );
}
    
@end
