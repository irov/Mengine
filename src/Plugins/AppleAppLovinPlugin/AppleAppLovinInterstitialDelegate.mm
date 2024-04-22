#import "AppleAppLovinInterstitialDelegate.h"

#include "Environment/Apple/AppleDetail.h"
#include "Environment/Apple/AppleString.h"
#include "Environment/Apple/AppleAnalytics.h"

#include "AppleAppLovinApplicationDelegate.h"

#include "Kernel/Logger.h"
#include "Kernel/AnalyticsHelper.h"

@implementation AppleAppLovinInterstitialDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                       amazonSlotId:(NSString * _Nullable) amazonSlotId
                                           provider:(const Mengine::AppleAppLovinInterstitialProviderInterfacePtr &) provider
                                          analytics:(AppleAppLovinAnalyticsService * _Nonnull) analytics {
    self = [super initWithAdUnitIdentifier:adUnitId adFormat:MAAdFormat.interstitial analytics:analytics];
    
    self.m_provider = provider;
    
    MAInterstitialAd * interstitialAd;
    
    @try {
        interstitialAd = [[MAInterstitialAd alloc] initWithAdUnitIdentifier:adUnitId];
    } @catch (NSException * ex) {
        LOGGER_ERROR( "[Error] AppleAppLovinInterstitialDelegate invalid create MAInterstitialAd adUnitId: %s exception: %s [%s]"
            , [adUnitId UTF8String]
            , [ex.reason UTF8String]
            , [ex.name UTF8String]
        );
        
        return nil;
    }
    
    interstitialAd.delegate = self;
    interstitialAd.revenueDelegate = self;
    interstitialAd.requestDelegate = self;
    interstitialAd.adReviewDelegate = self;
    
    self.m_interstitialAd = interstitialAd;
    
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
}

- (BOOL) canYouShow:(NSString * _Nonnull) placement {
    BOOL ready = [self.m_interstitialAd isReady];
    
    [self log:@"canYouShow" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    if( ready == NO )
    {
        [AppleAnalytics event:@"mng_ad_interstitial_show" params:@{
            @"ad_unit_id": self.m_adUnitId,
            @"request_id": @(self.m_requestId),
            @"placement": placement,
            @"attempt": @(self.m_retryAttempt),
            @"ready": @(NO)
        }];
        
        return NO;
    }
    
    return YES;
}

- (BOOL) show:(NSString * _Nonnull) placement {
    BOOL ready = [self.m_interstitialAd isReady];
       
    [self log:@"show" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    [AppleAnalytics event:@"mng_ad_interstitial_show" params:@{
        @"ad_unit_id": self.m_adUnitId,
        @"request_id": @(self.m_requestId),
        @"placement": placement,
        @"attempt": @(self.m_retryAttempt),
        @"ready": @(ready)
    }];
    
    if( ready == NO )
    {
        return NO;
    }
    
    [self.m_interstitialAd showAdForPlacement:placement];

    return YES;
}

- (void) loadAd {
    if( self.m_interstitialAd == nil )
    {
        return;
    }

    self.m_requestId = self.m_enumeratorRequest++;
    
    [self log:@"loadAd"];
    
    [AppleAnalytics event:@"mng_ad_interstitial_load" params:@{
        @"ad_unit_id": self.m_adUnitId,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt)
    }];
    
    [self.m_interstitialAd loadAd];
}

#pragma mark - MAAdRequestDelegate Protocol

- (void)didStartAdRequestForAdUnitIdentifier:(NSString *)adUnitIdentifier {
    [self log:@"didStartAdRequestForAdUnitIdentifier"];
    
    [AppleAnalytics event:@"mng_ad_interstitial_request_started" params:@{
        @"ad_unit_id": adUnitIdentifier,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt)        
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidStartAdRequestForAdUnitIdentifier();
}

#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *) ad {
    [self log:@"didLoadAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_interstitial_loaded" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidLoadAd();
    
    self.m_retryAttempt = 0;
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *) adUnitIdentifier withError:(MAError *) error {
    [self log:@"didFailToLoadAdForAdUnitIdentifier" withMAError:error];
    
    [AppleAnalytics event:@"mng_ad_interstitial_load_failed" params:@{
        @"ad_unit_id": adUnitIdentifier,
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code)
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidFailToLoadAdForAdUnitIdentifier();
    
    [self retryLoadAd];
}

- (void) didDisplayAd:(MAAd *) ad {
    [self log:@"didDisplayAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_interstitial_displayed" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidDisplayAd();
}

- (void) didClickAd:(MAAd *) ad {
    [self log:@"didClickAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_interstitial_clicked" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidClickAd();
}

- (void) didHideAd:(MAAd *) ad {
    [self log:@"didHideAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_interstitial_hidden" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidHideAd();
    
    [self loadAd];
}

- (void) didFailToDisplayAd:(MAAd *) ad withError:(MAError *) error {
    [self log:@"didFailToDisplayAd" withMAAd:ad withMAError:error];
        
    [AppleAnalytics event:@"mng_ad_interstitial_display_failed" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidFailToDisplayAd();
    
    [self loadAd];
}

#pragma mark - Revenue Callbacks

- (void)didPayRevenueForAd:(MAAd *) ad {
    [self log:@"didPayRevenueForAd" withMAAd:ad];
    
    [AppleAnalytics event:@"mng_ad_interstitial_revenue_paid" params:@{
        @"ad_unit_id": ad.adUnitIdentifier,
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"revenue_value": @(ad.revenue),
        @"revenue_precision": ad.revenuePrecision,
        @"ad": [self getMAAdParams:ad]
    }];

    Mengine::Params params;
    Mengine::Helper::AppleGetParamsFromNSDictionary( @{@"revenue":@(ad.revenue)}, &params );
    
    self.m_provider->onAppleAppLovinInterstitialDidPayRevenueForAd( params );
}

#pragma mark - AdReview Callbacks

- (void)didGenerateCreativeIdentifier:(NSString *)creativeIdentifier forAd:(MAAd *)ad {
    //ToDo
}

@end
