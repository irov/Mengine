#import "AppleAppLovinBannerDelegate.h"

@implementation AppleAppLovinBannerDelegate

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString* _Nonnull)AdUnitIdentifier
                                amazonBannerSlotId:(NSString * _Nullable) amazonAdSlotId
                                              rect:(CGRect) rect {
    self = [super init];
    
    self.m_adView = [[MAAdView alloc] initWithAdUnitIdentifier: AdUnitIdentifier];
    self.m_adView.delegate = self;

    self.m_adView.frame = rect;

    // Set background or background color for banners to be fully functional
    self.m_adView.backgroundColor = UIColor.blackColor;

    [self.rootViewController.view addSubview: self.m_adView];
    
    if( amazonAdSlotId != NULL && amazonAdSlotId.length>0){
        DTBAdSize *size = [[DTBAdSize alloc] initBannerAdSizeWithWidth: rect.size.width
                                                                height: rect.size.height
                                                           andSlotUUID: amazonAdSlotId];
        
        DTBAdLoader *adLoader = [[DTBAdLoader alloc] init];
        [adLoader setAdSizes: @[size]];
        [adLoader loadAd: self];
    }else{
        // Load the ad
        [self.m_adView loadAd];
    }
    
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

- (UIViewController* _Nullable) rootViewController {
    UIWindow * appWindow = [UIApplication sharedApplication].delegate.window;
    
    UIViewController* controller = appWindow.rootViewController;
    
    return controller;
}

#pragma mark - DTBAdCallback Protocol

-(void)onFailure: (DTBAdError)error{}

-(void)onFailure: (DTBAdError)error
  dtbAdErrorInfo:(DTBAdErrorInfo *) errorInfo{
    // 'adView' is your instance of MAAdView
    [self.m_adView setLocalExtraParameterForKey: @"amazon_ad_error" value: errorInfo];
    [self.m_adView loadAd];
}

- (void)onSuccess:(DTBAdResponse *)adResponse{
    // 'adView' is your instance of MAAdView
    [self.m_adView setLocalExtraParameterForKey: @"amazon_ad_response" value: adResponse];
    [self.m_adView loadAd];
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
