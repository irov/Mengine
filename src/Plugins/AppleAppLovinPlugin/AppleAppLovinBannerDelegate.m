#import "AppLovinBannerDelegate.h"

@implementation AppLovinBannerDelegate


- (instancetype _Nonnull) initWithRect:(CGRect) bannerRect key:(NSString* _Nonnull) key{
    self = [super init];
    
    self.adView = [[MAAdView alloc] initWithAdUnitIdentifier: key];
    self.adView.delegate = self;

    self.adView.frame = bannerRect;//CGRectMake(x, y, width, height);

    // Set background or background color for banners to be fully functional
    self.adView.backgroundColor = UIColor.blackColor;

    [self.rootViewConstroller addSubview: self.adView];

    // Load the ad
    [self.adView loadAd];
    
    return self;
}

- (void) show {
    self.adView.hidden = NO;
    [self.adView startAutoRefresh];
}

- (void) hide {
    self.adView.hidden = YES;
    [self.adView stopAutoRefresh];
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
}

- (void)didFailToLoadAdForAdUnitIdentifier:(NSString *)adUnitIdentifier withError:(MAError *)error {}

- (void)didClickAd:(MAAd *)ad {}

- (void)didFailToDisplayAd:(MAAd *)ad withError:(MAError *)error {}


#pragma mark - MAAdViewAdDelegate Protocol

- (void)didExpandAd:(MAAd *)ad {}

- (void)didCollapseAd:(MAAd *)ad {}

#pragma mark - Deprecated Callbacks

- (void)didDisplayAd:(MAAd *)ad { /* DO NOT USE - THIS IS RESERVED FOR FULLSCREEN ADS ONLY AND WILL BE REMOVED IN A FUTURE SDK RELEASE */ }
- (void)didHideAd:(MAAd *)ad { /* DO NOT USE - THIS IS RESERVED FOR FULLSCREEN ADS ONLY AND WILL BE REMOVED IN A FUTURE SDK RELEASE */ }

    
@end
