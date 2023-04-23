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
    
    self.m_retryAttempt = 0;
    self.m_enumeratorRequest = 0;
    self.m_requestId = 0;
    
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
    self.m_amazonLoader = [[AppleAppLovinInterstitialAmazonLoader alloc] initWithSlotId:amazonSlotId interstitialAd:self.m_interstitialAd];
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
    
    self.m_interstitialAd = nil;
    
    [super dealloc];
}

- (BOOL) canYouShow {
    BOOL ready = [self.m_interstitialAd isReady];
    
    return ready;
}

- (BOOL) show {
    BOOL ready = [self.m_interstitialAd isReady];
    
    LOGGER_MESSAGE("show interstitial request: %ld ready: %d"
        , self.m_requestId
        , ready
        );
    
    if( ready == NO )
    {
        return NO;
    }
    
    [self.m_interstitialAd showAd];

    return YES;
}

- (void) loadAd {
    if( self.m_interstitialAd == nil )
    {
        return;
    }
    
    LOGGER_MESSAGE( "load interstitial" );
    
    self.m_requestId = self.m_enumeratorRequest++;
    
    [self.m_interstitialAd loadAd];
}

#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *) ad {
    self.m_retryAttempt = 0;
    
    LOGGER_MESSAGE( "interstitial didLoadAd" );
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *) adUnitIdentifier withError:(MAError *) error {
    LOGGER_MESSAGE( "interstitial didFailToLoadAdForAdUnitIdentifier" );
    
    self.m_retryAttempt++;
    
    NSInteger delaySec = pow(2, MIN(6, self.m_retryAttempt));
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, delaySec * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self loadAd];
    });
}

- (void) didDisplayAd:(MAAd *) ad {
    LOGGER_MESSAGE( "interstitial didDisplayAd" );
}

- (void) didClickAd:(MAAd *) ad {
    LOGGER_MESSAGE( "interstitial didClickAd" );
}

- (void) didHideAd:(MAAd *) ad {
    LOGGER_MESSAGE( "interstitial didHideAd" );
    
    [self loadAd];
}

- (void) didFailToDisplayAd:(MAAd *) ad withError:(MAError *) error {
    LOGGER_MESSAGE( "interstitial didFailToDisplayAd" );
    
    [self loadAd];
}

#pragma mark - Revenue Callbacks

- (void)didPayRevenueForAd:(MAAd *)ad {
    LOGGER_MESSAGE( "interstitial didPayRevenueForAd" );
}

@end
