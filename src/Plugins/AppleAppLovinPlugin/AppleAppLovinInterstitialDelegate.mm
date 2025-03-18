#import "AppleAppLovinInterstitialDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleString.h"

#import "Environment/iOS/iOSLog.h"

#include "AppleAppLovinApplicationDelegate.h"

#include "Kernel/AnalyticsHelper.h"

@implementation AppleAppLovinInterstitialDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull)adUnitId {
    self = [super initWithAdUnitIdentifier:adUnitId adFormat:MAAdFormat.interstitial];
        
    MAInterstitialAd * interstitialAd;
    
    @try {
        interstitialAd = [[MAInterstitialAd alloc] initWithAdUnitIdentifier:adUnitId];
    } @catch (NSException * ex) {
        IOS_LOGGER_MESSAGE(@"[Error] AppleAppLovinInterstitialDelegate invalid create MAInterstitialAd adUnitId: %@ exception: %@ [%@]"
            , adUnitId
            , ex.reason
            , ex.name
        );
        
        return nil;
    }
    
    interstitialAd.delegate = self;
    interstitialAd.revenueDelegate = self;
    interstitialAd.requestDelegate = self;
    interstitialAd.adReviewDelegate = self;
    
    self.m_interstitialAd = interstitialAd;
    
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
    self.m_amazonLoader = [[AppleAppLovinInterstitialAmazonLoader alloc] initWithSlotId:amazonSlotId interstitialAd:self.m_interstitialAd];
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
    
    self.m_interstitialAd = nil;
}

- (BOOL) canYouShow:(NSString * _Nonnull)placement {
    if (self.m_interstitialAd == nil) {
        return NO;
    }
    
    BOOL ready = [self.m_interstitialAd isReady];
    
    [self log:@"canYouShow" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    if (ready == NO) {
        [self event:@"mng_ad_interstitial_show" params:@{
            @"placement": placement,
            @"ready": @(NO)
        }];
        
        return NO;
    }
    
    return YES;
}

- (BOOL) show:(NSString * _Nonnull)placement {
    if (self.m_interstitialAd == nil) {
        return NO;
    }
    
    BOOL ready = [self.m_interstitialAd isReady];
       
    [self log:@"show" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    [self event:@"mng_ad_interstitial_show" params:@{
        @"placement": placement,
        @"ready": @(ready)
    }];
    
    if (ready == NO) {
        return NO;
    }
    
    [self.m_interstitialAd showAdForPlacement:placement];

    return YES;
}

- (void) loadAd {
    if( self.m_interstitialAd == nil ) {
        return;
    }

    [self increaseRequestId];
    
    [self log:@"loadAd"];
    
    [self event:@"mng_ad_interstitial_load" params:@{}];
    
    [self.m_interstitialAd loadAd];
}

#pragma mark - MAAdRequestDelegate Protocol

- (void) didStartAdRequestForAdUnitIdentifier:(NSString *)adUnitIdentifier {
    [self log:@"didStartAdRequestForAdUnitIdentifier"];
    
    [self event:@"mng_ad_interstitial_request_started" params:@{}];
}

#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *)ad {
    [self log:@"didLoadAd" withMAAd:ad];
    
    [self event:@"mng_ad_interstitial_loaded" params:@{
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_requestAttempt = 0;
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *)adUnitIdentifier withError:(MAError *)error {
    [self log:@"didFailToLoadAdForAdUnitIdentifier" withMAError:error];
    
    [self event:@"mng_ad_interstitial_load_failed" params:@{
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code)
    }];
    
    [self retryLoadAd];
}

- (void) didDisplayAd:(MAAd *)ad {
    [self log:@"didDisplayAd" withMAAd:ad];
    
    [self event:@"mng_ad_interstitial_displayed" params:@{
        @"placement": ad.placement,
        @"ad": [self getMAAdParams:ad]
    }];
}

- (void) didClickAd:(MAAd *)ad {
    [self log:@"didClickAd" withMAAd:ad];
    
    [self event:@"mng_ad_interstitial_clicked" params:@{
        @"placement": ad.placement,
        @"ad": [self getMAAdParams:ad]
    }];
}

- (void) didHideAd:(MAAd *)ad {
    [self log:@"didHideAd" withMAAd:ad];
    
    [self event:@"mng_ad_interstitial_hidden" params:@{
        @"placement": ad.placement,
        @"ad": [self getMAAdParams:ad]
    }];
    
    id<AppleAdvertisementCallbackInterface> callback = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementInterstitialCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementShowSuccess:ad.placement];
    }
    
    [self loadAd];
}

- (void) didFailToDisplayAd:(MAAd *)ad withError:(MAError *)error {
    [self log:@"didFailToDisplayAd" withMAAd:ad withMAError:error];
        
    [self event:@"mng_ad_interstitial_display_failed" params:@{
        @"placement": ad.placement,
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code),
        @"ad": [self getMAAdParams:ad]
    }];
    
    id<AppleAdvertisementCallbackInterface> callback = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementInterstitialCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementShowFailed:ad.placement withError:error.code];
    }
    
    [self loadAd];
}

#pragma mark - Revenue Callbacks

- (void)didPayRevenueForAd:(MAAd *) ad {
    [self log:@"didPayRevenueForAd" withMAAd:ad];
    
    [self event:@"mng_ad_interstitial_revenue_paid" params:@{
        @"placement": ad.placement,
        @"revenue_value": @(ad.revenue),
        @"revenue_precision": ad.revenuePrecision,
        @"ad": [self getMAAdParams:ad]
    }];
    
    [self eventRevenue:ad];
    
    id<AppleAdvertisementCallbackInterface> callback = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementInterstitialCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementRevenuePaid:ad.placement withRevenue:ad.revenue];
    }
}

#pragma mark - AdReview Callbacks

- (void)didGenerateCreativeIdentifier:(NSString *)creativeIdentifier forAd:(MAAd *)ad {
    //ToDo
}

@end
