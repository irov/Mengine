#import "AppleAppLovinBannerDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleString.h"
#import "Environment/Apple/AppleAnalytics.h"

#import "Environment/iOS/iOSDetail.h"

#include "AppleAppLovinApplicationDelegate.h"

#include "Kernel/Logger.h"

@implementation AppleAppLovinBannerDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                          placement:(NSString * _Nonnull) placement
                                       amazonSlotId:(NSString * _Nullable) amazonSlotId
                                           provider:(const Mengine::AppleAppLovinBannerProviderInterfacePtr &) provider {
    self = [super initWithAdUnitIdentifier:adUnitId adFormat:MAAdFormat.banner];
    
    self.m_provider = provider;
    
    BOOL MengineAppleAppLovinPlugin_BannerAdaptive = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"BannerAdaptive" withDefault:YES];
    
    m_bannerAdaptive = MengineAppleAppLovinPlugin_BannerAdaptive;
    
    MAAdView * adView;
    
    @try {
        adView = [[MAAdView alloc] initWithAdUnitIdentifier:adUnitId];
    } @catch (NSException * ex) {
        LOGGER_ERROR( "[Error] AppleAppLovinBannerDelegate invalid create MAAdView adUnitId: %s exception: %s [%s]"
            , [adUnitId UTF8String]
            , [ex.reason UTF8String]
            , [ex.name UTF8String]
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
    
    if (m_bannerAdaptive == YES) {
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
    if( self.m_adView == nil ) {
        return;
    }
    
    self.m_requestId = self.m_enumeratorRequest++;
    
    [AppleAnalytics event:@"mng_ad_banner_load" params:@{
        @"ad_unit_id": self.m_adUnitId,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt)
    }];
    
    [self.m_adView loadAd];
}

- (CGSize) getSize {
    if (m_bannerAdaptive == YES) {
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
    if( self.m_adView == nil ) {
        return CGRectZero;
    }
    
    CGRect frame = self.m_adView.frame;
    
    return frame;
}

- (CGFloat) getHeight {
    CGSize banner_size = [self getSize];
    
    CGFloat banner_heightDp = banner_size.height;
    CGFloat banner_scale = UIScreen.mainScreen.scale;

    CGFloat banner_heightPx = banner_heightDp * banner_scale;

    return banner_heightPx;
}

#pragma mark - MAAdRequestDelegate Protocol

- (void) didStartAdRequestForAdUnitIdentifier:(NSString *)adUnitIdentifier {
    [self log:@"didStartAdRequestForAdUnitIdentifier"];
    
    [AppleAnalytics event:@"mng_ad_banner_request_started" params:@{
        @"ad_unit_id": adUnitIdentifier,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt)
    }];
    
    self.m_provider->onAppleAppLovinBannerDidStartAdRequestForAdUnitIdentifier();
}

#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *) ad {
    [self log:@"didLoadAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_banner_loaded" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_retryAttempt = 0;
    
    self.m_provider->onAppleAppLovinBannerDidLoadAd();
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *) adUnitIdentifier withError:(MAError *) error {
    [self log:@"didFailToLoadAdForAdUnitIdentifier" withMAError:error];
    
    [AppleAnalytics event:@"mng_ad_banner_load_failed" params:@{
        @"ad_unit_id": adUnitIdentifier,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code)
    }];
    
    self.m_retryAttempt++;
    
    self.m_provider->onAppleAppLovinBannerDidFailToLoadAdForAdUnitIdentifier();
}

- (void) didDisplayAd:(MAAd *)ad
{
    [self log:@"didDisplayAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_banner_displayed" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt)
    }];
    
    self.m_provider->onAppleAppLovinBannerDidDisplayAd();
}

- (void) didHideAd:(MAAd *)ad
{
    [self log:@"didHideAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_banner_hidden" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt)
    }];
    
    self.m_provider->onAppleAppLovinBannerDidHideAd();
}

- (void) didClickAd:(MAAd *) ad {
    [self log:@"didClickAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_banner_clicked" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinBannerDidClickAd();
}

- (void) didFailToDisplayAd:(MAAd *) ad withError:(MAError *) error {
    [self log:@"didFailToDisplayAd" withMAAd:ad withMAError:error];
    
    [AppleAnalytics event:@"mng_ad_banner_display_failed" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinBannerDidFailToDisplayAd();
}


#pragma mark - MAAdViewAdDelegate Protocol

- (void) didExpandAd:(MAAd *) ad {
    [self log:@"didExpandAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_banner_expand" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinBannerDidExpandAd();
}

- (void) didCollapseAd:(MAAd *) ad {
    [self log:@"didCollapseAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_banner_collapse" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinBannerDidCollapseAd();
}
    
#pragma mark - Revenue Callbacks

- (void) didPayRevenueForAd:(MAAd *)ad {
    [self log:@"didPayRevenueForAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_banner_revenue_paid" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"revenue_value": @(ad.revenue),
        @"revenue_precision": ad.revenuePrecision,
        @"ad": [self getMAAdParams:ad]
    }];
    
    [self eventRevenue:ad];
    
    Mengine::Params params;
    [AppleDetail getParamsFromNSDictionary:@{@"revenue":@(ad.revenue)} outParams:&params];
    
    self.m_provider->onAppleAppLovinBannerDidPayRevenueForAd( params );
}

#pragma mark - AdReview Callbacks

- (void) didGenerateCreativeIdentifier:(NSString *)creativeIdentifier forAd:(MAAd *)ad {
    //ToDo
}

@end
