#import "AppLovinRewardedDelegate.h"

@implementation AppLovinRewardedDelegate

@synthesize m_callbacks;

- (instancetype _Nonnull) initWithCallback:(id<AppLovinRewardCallbackProtocol> * _Nonnull)callback AdUnitIdentifier:(NSString* _Nonnull) key {
    self = [super init];
    
    self.m_callbacks = callback;
    self.rewardedAd = [MARewardedAd sharedWithAdUnitIdentifier: key];
    self.rewardedAd.delegate = self;
    
    // Load the first ad
    [self.rewardedAd loadAd];
    
    return self;
}

- (BOOL) hasLoaded {
    return [self.rewardedAd isReady];
}

- (BOOL) show {
    if( self.hasLoaded ){
        [self.rewardedAd showAd];
        return YES;
    }
    return NO;
}

#pragma mark - MAAdDelegate Protocol

- (void)didLoadAd:(MAAd *)ad
{
    // Rewarded ad is ready to be shown. '[self.rewardedAd isReady]' will now return 'YES'
    
    // Reset retry attempt
    self.retryAttempt = 0;
}

- (void)didFailToLoadAdForAdUnitIdentifier:(NSString *)adUnitIdentifier withError:(MAError *)error
{
    // Rewarded ad failed to load
    // We recommend retrying with exponentially higher delays up to a maximum delay (in this case 64 seconds)
    
    self.retryAttempt++;
    NSInteger delaySec = pow(2, MIN(6, self.retryAttempt));
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, delaySec * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self.rewardedAd loadAd];
    });
}

- (void)didDisplayAd:(MAAd *)ad {}

- (void)didClickAd:(MAAd *)ad {}

- (void)didHideAd:(MAAd *)ad
{
    // Rewarded ad is hidden. Pre-load the next ad
    [self.rewardedAd loadAd];
}

- (void)didFailToDisplayAd:(MAAd *)ad withError:(MAError *)error
{
    // Rewarded ad failed to display. We recommend loading the next ad
    [self.rewardedAd loadAd];
}

#pragma mark - MARewardedAdDelegate Protocol

- (void)didStartRewardedVideoForAd:(MAAd *)ad {}

- (void)didCompleteRewardedVideoForAd:(MAAd *)ad {}

- (void)didRewardUserForAd:(MAAd *)ad withReward:(MAReward *)reward
{
    // Rewarded ad was displayed and user should receive the reward
    NSLog(@"Rewarded user: %d %@", reward.amount, reward.label);
    [self.m_callbacks receivedReward:reward.amount];
}
    
@end
