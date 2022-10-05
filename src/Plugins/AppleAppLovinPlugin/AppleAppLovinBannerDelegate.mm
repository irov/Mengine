#import "AppleAppLovinBannerDelegate.h"

#include "Environment/iOS/iOSDetail.h"

@implementation AppleAppLovinBannerDelegate

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitIdentifier
                                      amazonSlotId:(NSString * _Nullable) amazonSlotId
                                              rect:(CGRect) rect {
    self = [super init];
    
    self.m_adView = [[MAAdView alloc] initWithAdUnitIdentifier: adUnitIdentifier];
    self.m_adView.delegate = self;

    self.m_adView.frame = rect;

    // Set background or background color for banners to be fully functional
    self.m_adView.backgroundColor = UIColor.blackColor;

    [self.rootViewController.view addSubview: self.m_adView];
    
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
    if( [amazonSlotId length] != 0 ) {
        self.m_amazonLoader = [[AppleAppLovinBannerAmazonLoader alloc] initWithSlotId: amazonSlotId adView: self.m_adView rect: rect];
    } else {
        // Load the ad
        [self.m_adView loadAd];
    }
#else
    [self.m_adView loadAd];
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
    
    [super dealloc];
}

- (void) show {
    self.m_adView.hidden = NO;

    [self.m_adView startAutoRefresh];
}

- (void) hide {
    self.m_adView.hidden = YES;

    [self.m_adView stopAutoRefresh];
}

- (UIViewController* _Nullable) rootViewController {
    UIViewController * rootViewController = Helper::iOSGetRootViewController();
    
    return controller;
}

#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *) ad {
    CGSize adViewSize = ad.size;
    CGFloat widthDp = adViewSize.width;
    CGFloat heightDp = adViewSize.height;

    //ToDo
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *) adUnitIdentifier withError:(MAError *) error {}

- (void) didClickAd:(MAAd *) ad {}

- (void) didFailToDisplayAd:(MAAd *) ad withError:(MAError *) error {}


#pragma mark - MAAdViewAdDelegate Protocol

- (void) didExpandAd:(MAAd *) ad {}

- (void) didCollapseAd:(MAAd *) ad {}

#pragma mark - Deprecated Callbacks

/* DO NOT USE - THIS IS RESERVED FOR FULLSCREEN ADS ONLY AND WILL BE REMOVED IN A FUTURE SDK RELEASE */
- (void) didDisplayAd:(MAAd *) ad {}

- (void) didHideAd:(MAAd *) ad {}
/* DO NOT USE - THIS IS RESERVED FOR FULLSCREEN ADS ONLY AND WILL BE REMOVED IN A FUTURE SDK RELEASE */
    
@end
