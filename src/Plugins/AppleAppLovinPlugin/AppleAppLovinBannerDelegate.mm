#import "AppleAppLovinBannerDelegate.h"

#include "Environment/Apple/AppleDetail.h"
#include "Environment/Apple/AppleString.h"
#include "Environment/Apple/AppleAnalytics.h"

#include "Environment/iOS/iOSDetail.h"

#include "AppleAppLovinApplicationDelegate.h"

#include "Kernel/Logger.h"

@implementation AppleAppLovinBannerDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                          placement:(NSString * _Nonnull) placement
                                       amazonSlotId:(NSString * _Nullable) amazonSlotId
                                               rect:(CGRect) rect
                                           provider:(const Mengine::AppleAppLovinBannerProviderInterfacePtr &) provider
                                          analytics:(AppleAppLovinAnalyticsService * _Nonnull) analytics {
    self = [super initWithAdUnitIdentifier:adUnitId adFormat:MAAdFormat.banner analytics:analytics];
    
    self.m_provider = provider;
    
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

    adView.frame = rect;
    adView.backgroundColor = UIColor.clearColor;

    UIViewController * rootViewController = Mengine::Helper::iOSGetRootViewController();
    [rootViewController.view addSubview:adView];
    adView.hidden = YES;
    
    self.m_adView = adView;
    
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
    self.m_amazonLoader = [[AppleAppLovinBannerAmazonLoader alloc] initWithSlotId:amazonSlotId adView:self.m_adView rect:rect];
#else
    [self loadAd];
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
    
    self.m_adView = nil;
}

- (void) show {
    self.m_adView.hidden = NO;

    [self.m_adView startAutoRefresh];
}

- (void) hide {
    self.m_adView.hidden = YES;

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

- (CGRect) getRect {
    if( self.m_adView == nil ) {
        return CGRectZero;
    }
    
    CGRect frame = self.m_adView.frame;
    
    return frame;
}

#pragma mark - MAAdRequestDelegate Protocol

- (void)didStartAdRequestForAdUnitIdentifier:(NSString *)adUnitIdentifier {
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

#pragma mark - Deprecated Callbacks

- (void) didDisplayAd:(MAAd *) ad {
    [self log:@"didDisplayAd" withMAAd:ad];
    
    //deprecated
}

- (void) didHideAd:(MAAd *) ad {
    [self log:@"didHideAd" withMAAd:ad];
    
    //deprecated
}
    
#pragma mark - Revenue Callbacks

- (void)didPayRevenueForAd:(MAAd *)ad {
    [self log:@"didPayRevenueForAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_banner_revenue_paid" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"revenue_value": @(ad.revenue),
        @"revenue_precision": ad.revenuePrecision,
        @"ad": [self getMAAdParams:ad]
    }];
    
    [self.m_analytics eventRevenuePaid:ad];
    
    Mengine::Params params;
    Mengine::Helper::AppleGetParamsFromNSDictionary( @{@"revenue":@(ad.revenue)}, &params );
    
    self.m_provider->onAppleAppLovinBannerDidPayRevenueForAd( params );
}

#pragma mark - AdReview Callbacks

- (void)didGenerateCreativeIdentifier:(NSString *)creativeIdentifier forAd:(MAAd *)ad {
    //ToDo
}

@end
