#import "AppleAppLovinBannerDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleString.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#import "AppleAppLovinApplicationDelegate.h"

@implementation AppleAppLovinBannerDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                          placement:(NSString * _Nonnull) placement
                                           adaptive:(BOOL) adaptive {
    self = [super initWithAdUnitIdentifier:adUnitId adFormat:MAAdFormat.banner];
    
    self.m_bannerAdaptive = adaptive;
    
    MAAdView * adView;
    
    @try {
        adView = [[MAAdView alloc] initWithAdUnitIdentifier:adUnitId];
    } @catch (NSException * ex) {
        IOS_LOGGER_MESSAGE(@"[Error] AppleAppLovinBannerDelegate invalid create MAAdView adUnitId: %@ exception: %@ [%@]"
            , adUnitId
            , ex.reason
            , ex.name
        );
        
        return nil;
    }
    
    [adView setPlacement:placement];
    
    adView.delegate = self;
    adView.revenueDelegate = self;
    adView.requestDelegate = self;
    adView.adReviewDelegate = self;
    
    CGSize size = [self getSize];
    
    CGFloat banner_height = size.height;
    
    CGFloat screen_width = CGRectGetWidth(UIScreen.mainScreen.bounds);
    CGFloat screen_height = CGRectGetHeight(UIScreen.mainScreen.bounds);
    
    CGRect rect = CGRectMake(0, screen_height - banner_height, screen_width, banner_height);
    
    adView.frame = rect;
    
    adView.backgroundColor = UIColor.clearColor;
    
    if (self.m_bannerAdaptive == YES) {
        [adView setExtraParameterForKey:@"adaptive_banner" value:@"true"];
    }

    UIViewController * viewController = [iOSDetail getRootViewController];
    [viewController.view addSubview:adView];
    adView.hidden = YES;
    
    self.m_adView = adView;
    
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
    self.m_amazonLoader = [[AppleAppLovinBannerAmazonLoader alloc] initWithSlotId:amazonSlotId adView:self.m_adView rect:rect];
#else
    [self loadAd];
#endif
    
    return self;
}

- (void) dealloc {
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
    if (self.m_amazonLoader != nil) {
        [self.m_amazonLoader release];
        self.m_amazonLoader = nil;
    }
#endif
    
    if (self.m_adView != nil) {
        self.m_adView.delegate = nil;
        self.m_adView.revenueDelegate = nil;
        self.m_adView.requestDelegate = nil;
        self.m_adView.adReviewDelegate = nil;
        
        [self.m_adView removeFromSuperview];
        self.m_adView = nil;
    }
}

- (void) show {
    self.m_adView.hidden = NO;

    [self.m_adView startAutoRefresh];
}

- (void) hide {
    self.m_adView.hidden = YES;

    [self.m_adView setExtraParameterForKey:@"allow_pause_auto_refresh_immediately" value:@"true"];
    [self.m_adView stopAutoRefresh];
}

- (void) loadAd {
    if (self.m_adView == nil) {
        return;
    }
    
    [self increaseRequestId];
    
    [self log:@"loadAd"];
    
    [self event:@"mng_ad_banner_load" params:@{}];
    
    [self.m_adView loadAd];
}

- (CGSize) getSize {
    if (self.m_bannerAdaptive == YES) {
        return MAAdFormat.banner.adaptiveSize;
    } else {
        BOOL isPad = UIDevice.currentDevice.userInterfaceIdiom == UIUserInterfaceIdiomPad;
        
        if (isPad == YES) {
            return MAAdFormat.leader.size;
        } else {
            return MAAdFormat.banner.size;
        }
    }
}

- (CGRect) getRect {
    if (self.m_adView == nil) {
        return CGRectZero;
    }
    
    CGRect frame = self.m_adView.frame;
    
    return frame;
}

- (CGFloat) getHeightPx {
    CGSize banner_size = [self getSize];
    
    CGFloat banner_heightDp = banner_size.height;
    CGFloat banner_scale = UIScreen.mainScreen.scale;

    CGFloat banner_heightPx = banner_heightDp * banner_scale;

    return banner_heightPx;
}

- (CGFloat) getWidthPx {
    CGSize banner_size = [self getSize];
    
    CGFloat banner_widthDp = banner_size.width;
    CGFloat banner_scale = UIScreen.mainScreen.scale;

    CGFloat banner_widthPx = banner_widthDp * banner_scale;

    return banner_widthPx;
}

#pragma mark - MAAdRequestDelegate Protocol

- (void) didStartAdRequestForAdUnitIdentifier:(NSString *)adUnitIdentifier {
    [self increaseRequestId];
    
    [self log:@"didStartAdRequestForAdUnitIdentifier"];
    
    [self event:@"mng_ad_banner_request_started" params:@{}];
}

#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *)ad {
    [self log:@"didLoadAd" withMAAd:ad];
    
    [self event:@"mng_ad_banner_loaded" params:@{
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_requestAttempt = 0;
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *)adUnitIdentifier withError:(MAError *)error {
    [self log:@"didFailToLoadAdForAdUnitIdentifier" withMAError:error];
    
    [self event:@"mng_ad_banner_load_failed" params:@{
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code)
    }];
    
    self.m_requestAttempt++;
}

- (void) didDisplayAd:(MAAd *)ad {
    [self log:@"didDisplayAd" withMAAd:ad];
    
    [self event:@"mng_ad_banner_displayed" params:@{
        @"ad": [self getMAAdParams:ad]
    }];
}

- (void) didHideAd:(MAAd *)ad {
    [self log:@"didHideAd" withMAAd:ad];
    
    [self event:@"mng_ad_banner_hidden" params:@{
        @"ad": [self getMAAdParams:ad]
    }];
}

- (void) didClickAd:(MAAd *)ad {
    [self log:@"didClickAd" withMAAd:ad];
    
    [self event:@"mng_ad_banner_clicked" params:@{
        @"ad": [self getMAAdParams:ad]
    }];
}

- (void) didFailToDisplayAd:(MAAd *)ad withError:(MAError *)error {
    [self log:@"didFailToDisplayAd" withMAAd:ad withMAError:error];
    
    [self event:@"mng_ad_banner_display_failed" params:@{
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code),
        @"ad": [self getMAAdParams:ad]
    }];
}


#pragma mark - MAAdViewAdDelegate Protocol

- (void) didExpandAd:(MAAd *)ad {
    [self log:@"didExpandAd" withMAAd:ad];
    
    [self event:@"mng_ad_banner_expand" params:@{
        @"ad": [self getMAAdParams:ad]
    }];
}

- (void) didCollapseAd:(MAAd *)ad {
    [self log:@"didCollapseAd" withMAAd:ad];
    
    [self event:@"mng_ad_banner_collapse" params:@{
        @"ad": [self getMAAdParams:ad]
    }];
}
    
#pragma mark - Revenue Callbacks

- (void) didPayRevenueForAd:(MAAd *)ad {
    [self log:@"didPayRevenueForAd" withMAAd:ad];
    
    [self event:@"mng_ad_banner_revenue_paid" params:@{
        @"revenue_value": @(ad.revenue),
        @"revenue_precision": ad.revenuePrecision,
        @"ad": [self getMAAdParams:ad]
    }];
    
    [self eventRevenue:ad];
    
    id<AppleAdvertisementResponseInterface> response = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementResponse];
    
    if (response != nil) {
        [response onAppleAdvertisementRevenuePaid:[iOSAdFormat ADFORMAT_BANNER] withPlacement:ad.placement withRevenue:ad.revenue];
    }
}

#pragma mark - AdReview Callbacks

- (void) didGenerateCreativeIdentifier:(NSString *)creativeIdentifier forAd:(MAAd *)ad {
    //ToDo
}

@end
