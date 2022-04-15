#import "AppleAppLovinBannerDelegate.h"

@implementation AppleAppLovinBannerDelegate


- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString* _Nonnull) AdUnitIdentifier rect:(CGRect) rect {
    self = [super init];
    
    self.m_adView = [[MAAdView alloc] initWithAdUnitIdentifier: AdUnitIdentifier];
    self.m_adView.delegate = self;

    self.m_adView.frame = rect;//CGRectMake(x, y, width, height);

    // Set background or background color for banners to be fully functional
    self.m_adView.backgroundColor = UIColor.blackColor;

    [self.rootViewConstroller addSubview: self.adView];

    // Load the ad
    [self.m_adView loadAd];
    
    return self;
}

- (void) show {
    self.m_adView.hidden = NO;

    [self.m_adView startAutoRefresh];
}

- (void) hide {
    self.m_adView.hidden = YES;

    [self.m_adView stopAutoRefresh];
}

- (UIViewController* _Nullable) rootViewConstroller {
    UIWindow * appWindow = [UIApplication sharedApplication].delegate.window;
    return appWindow.rootViewController;
}

#pragma mark - MAAdDelegate Protocol

- (void)didLoadAd:(MAAd *)ad {
    CGSize adViewSize = ad.size;
    CGFloat widthDp = adViewSize.width;
    CGFloat heightDp = adViewSize.height;

    //ToDo
}

- (void)didFailToLoadAdForAdUnitIdentifier:(NSString *)adUnitIdentifier withError:(MAError *)error {}

- (void)didClickAd:(MAAd *)ad {}

- (void)didFailToDisplayAd:(MAAd *)ad withError:(MAError *)error {}


#pragma mark - MAAdViewAdDelegate Protocol

- (void)didExpandAd:(MAAd *)ad {}

- (void)didCollapseAd:(MAAd *)ad {}

#pragma mark - Deprecated Callbacks

/* DO NOT USE - THIS IS RESERVED FOR FULLSCREEN ADS ONLY AND WILL BE REMOVED IN A FUTURE SDK RELEASE */
- (void)didDisplayAd:(MAAd *)ad {}
- (void)didHideAd:(MAAd *)ad {}
/* DO NOT USE - THIS IS RESERVED FOR FULLSCREEN ADS ONLY AND WILL BE REMOVED IN A FUTURE SDK RELEASE */
    
@end
