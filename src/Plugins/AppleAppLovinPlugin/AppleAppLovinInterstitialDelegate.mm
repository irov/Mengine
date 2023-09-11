#import "AppleAppLovinInterstitialDelegate.h"

#include "Environment/Apple/AppleString.h"
#include "Environment/Apple/AppleAnalytics.h"

#include "Kernel/Logger.h"
#include "Kernel/AnalyticsHelper.h"

@implementation AppleAppLovinInterstitialDelegate

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                      amazonSlotId:(NSString * _Nullable) amazonSlotId
                                          provider:(const Mengine::AppleAppLovinInterstitialProviderInterfacePtr &) provider
                                         analytics:(AppleAppLovinAnalyticsService * _Nonnull) analytics {
    self = [super initWithAdUnitIdentifier:adUnitId analytics:analytics];
    
    self.m_provider = provider;
    
    MAInterstitialAd * interstitialAd = [[MAInterstitialAd alloc] initWithAdUnitIdentifier:adUnitId];
    interstitialAd.delegate = self;
    interstitialAd.revenueDelegate = self;
    interstitialAd.requestDelegate = self;
    
    self.m_retryAttempt = 0;
    self.m_enumeratorRequest = 0;
    self.m_requestId = 0;
    
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
    
    LOGGER_MESSAGE("canYouShow interstitial placement: %s ready: %d request: %ld attempt: %ld"
        , [placement UTF8String]
        , ready
        , self.m_requestId
        , self.m_retryAttempt
        );
    
    if( ready == NO )
    {
        [AppleAnalytics event:@"mng_ad_interstitial_show" params:@{
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
    
    LOGGER_MESSAGE("show interstitial placement: %s ready: %d request: %ld attempt: %ld"
        , [placement UTF8String]
        , ready
        , self.m_requestId
        , self.m_retryAttempt
        );
    
    [AppleAnalytics event:@"mng_ad_interstitial_show" params:@{
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
    
    LOGGER_MESSAGE( "load interstitial request: %ld attempt: %ld"
        , self.m_requestId
        , self.m_retryAttempt
    );
    
    [AppleAnalytics event:@"mng_ad_interstitial_load" params:@{
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt)
    }];
    
    [self.m_interstitialAd loadAd];
}

#pragma mark - MAAdRequestDelegate Protocol

- (void)didStartAdRequestForAdUnitIdentifier:(NSString *)adUnitIdentifier {
    LOGGER_MESSAGE( "interstitial didStartAdRequestForAdUnitIdentifier: %s request: %ld attempt: %ld"
        , [adUnitIdentifier UTF8String]
        , self.m_requestId
        , self.m_retryAttempt
    );
    
    [AppleAnalytics event:@"mng_ad_interstitial_request_started" params:@{
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"unit_id": adUnitIdentifier
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidStartAdRequestForAdUnitIdentifier();
}

#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *) ad {
    LOGGER_MESSAGE( "interstitial didLoadAd: %s request: %ld attempt: %ld"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
        , self.m_retryAttempt
    );
    
    [AppleAnalytics event:@"mng_ad_interstitial_loaded" params:@{
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidLoadAd();
    
    self.m_retryAttempt = 0;
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *) adUnitIdentifier withError:(MAError *) error {
    LOGGER_MESSAGE( "interstitial didFailToLoadAdForAdUnitIdentifier: %s request: %ld attempt: %ld error: %s"
        , [adUnitIdentifier UTF8String]
        , self.m_requestId
        , self.m_retryAttempt
        , [[self getMAErrorParams:error] UTF8String]
    );
    
    [AppleAnalytics event:@"mng_ad_interstitial_load_failed" params:@{
        @"request_id": @(self.m_requestId),
        @"attempt": @(self.m_retryAttempt),
        @"unit_id": adUnitIdentifier,
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code)
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidFailToLoadAdForAdUnitIdentifier();
    
    self.m_retryAttempt++;
    
    NSInteger delaySec = pow(2, MIN(6, self.m_retryAttempt));
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, delaySec * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self loadAd];
    });
}

- (void) didDisplayAd:(MAAd *) ad {
    LOGGER_MESSAGE( "interstitial didDisplayAd: %s request: %ld"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
    );
    
    [AppleAnalytics event:@"mng_ad_interstitial_displayed" params:@{
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidDisplayAd();
}

- (void) didClickAd:(MAAd *) ad {
    LOGGER_MESSAGE( "interstitial didClickAd: %s request: %ld"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
    );
    
    [AppleAnalytics event:@"mng_ad_interstitial_clicked" params:@{
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidClickAd();
}

- (void) didHideAd:(MAAd *) ad {
    LOGGER_MESSAGE( "interstitial didHideAd: %s request: %ld"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
    );
    
    [AppleAnalytics event:@"mng_ad_interstitial_hidden" params:@{
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidHideAd();
    
    [self loadAd];
}

- (void) didFailToDisplayAd:(MAAd *) ad withError:(MAError *) error {
    LOGGER_MESSAGE( "interstitial didFailToDisplayAd: %s request: %ld error: %s"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
        , [[self getMAErrorParams:error] UTF8String]
    );
    
    [AppleAnalytics event:@"mng_ad_interstitial_display_failed" params:@{
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidFailToDisplayAd();
    
    [self loadAd];
}

#pragma mark - Revenue Callbacks

- (void)didPayRevenueForAd:(MAAd *)ad {
    LOGGER_MESSAGE( "interstitial didPayRevenueForAd: %s request: %ld"
        , [[self getMAAdParams:ad] UTF8String]
        , self.m_requestId
    );
    
    [AppleAnalytics event:@"mng_ad_interstitial_revenue_paid" params:@{
        @"placement": ad.placement,
        @"request_id": @(self.m_requestId),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_provider->onAppleAppLovinInterstitialDidPayRevenueForAd();
}

@end
