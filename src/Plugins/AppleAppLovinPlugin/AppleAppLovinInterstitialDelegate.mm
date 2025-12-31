#import "AppleAppLovinInterstitialDelegate.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/SoundServiceInterface.h"

#include "Kernel/ConstStringHelper.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleString.h"

#import "Environment/iOS/iOSLog.h"

#import "AppleAppLovinApplicationDelegate.h"

@implementation AppleAppLovinInterstitialDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull)adUnitId advertisement:(id<AppleAdvertisementInterface> _Nonnull)advertisement {
    self = [super initWithAdUnitIdentifier:adUnitId adFormat:MAAdFormat.interstitial advertisement:advertisement];
    
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
    interstitialAd.requestDelegate = self;
    interstitialAd.revenueDelegate = self;
    interstitialAd.expirationDelegate = self;
    interstitialAd.adReviewDelegate = self;
    
    self.m_interstitialAd = interstitialAd;
    
    self.m_showing = NO;
    
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
    
    if (self.m_interstitialAd != nil) {
        self.m_interstitialAd.delegate = nil;
        self.m_interstitialAd.requestDelegate = nil;
        self.m_interstitialAd.revenueDelegate = nil;
        self.m_interstitialAd.expirationDelegate = nil;
        self.m_interstitialAd.adReviewDelegate = nil;
        
        self.m_interstitialAd = nil;
    }
}

- (void) eventInterstitial:(NSString * _Nonnull) eventName params:(NSDictionary<NSString *, id> * _Nullable) params {
    [self event:[@"mng_applovin_interstitial_" stringByAppendingString:eventName] params:params];
}

- (BOOL) canYouShow:(NSString * _Nonnull)placement {
    if (self.m_interstitialAd == nil) {
        return NO;
    }
    
    BOOL ready = [self.m_interstitialAd isReady];
    
    [self log:@"canYouShow" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    if (ready == NO) {
        [self eventInterstitial:@"show" params:@{
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
    
    [self eventInterstitial:@"show" params:@{
        @"placement": placement,
        @"ready": @(ready)
    }];
    
    if (ready == NO) {
        return NO;
    }
    
    self.m_showing = YES;
    
    [self.m_interstitialAd showAdForPlacement:placement];

    return YES;
}

- (BOOL) isShowing {
    return self.m_showing;
}

- (void) loadAd {
    if( self.m_interstitialAd == nil ) {
        return;
    }

    [self increaseRequestId];
    
    [self log:@"loadAd"];
    
    [self eventInterstitial:@"load" params:@{}];
    
    [self.m_interstitialAd loadAd];
}

#pragma mark - MAAdRequestDelegate

- (void) didStartAdRequestForAdUnitIdentifier:(NSString *)adUnitIdentifier {
    [self log:@"didStartAdRequestForAdUnitIdentifier"];
    
    [self eventInterstitial:@"request_started" params:@{}];
}

#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *)ad {
    [self log:@"didLoadAd" withMAAd:ad];
    
    [self eventInterstitial:@"loaded" params:@{
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_requestAttempt = 0;
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *)adUnitIdentifier withError:(MAError *)error {
    [self log:@"didFailToLoadAdForAdUnitIdentifier" withMAError:error];
    
    [self eventInterstitial:@"load_failed" params:@{
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code)
    }];
    
    [self retryLoadAd];
}

- (void) didDisplayAd:(MAAd *)ad {
    [self log:@"didDisplayAd" withMAAd:ad];
    
    [self eventInterstitial:@"displayed" params:@{
        @"placement": ad.placement,
        @"ad": [self getMAAdParams:ad]
    }];
    
    [self setAdFreeze:YES];
}

- (void) didClickAd:(MAAd *)ad {
    [self log:@"didClickAd" withMAAd:ad];
    
    [self eventInterstitial:@"clicked" params:@{
        @"placement": ad.placement,
        @"ad": [self getMAAdParams:ad]
    }];
}

- (void) didHideAd:(MAAd *)ad {
    [self log:@"didHideAd" withMAAd:ad];
    
    [self eventInterstitial:@"hidden" params:@{
        @"placement": ad.placement,
        @"ad": [self getMAAdParams:ad]
    }];
    
    [self setAdFreeze:NO];
    
    self.m_showing = NO;
    
    id<AppleAdvertisementCallbackInterface> callback = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementInterstitialCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementShowSuccess:ad.placement];
    }
    
    [self loadAd];
}

- (void) didFailToDisplayAd:(MAAd *)ad withError:(MAError *)error {
    [self log:@"didFailToDisplayAd" withMAAd:ad withMAError:error];
        
    [self eventInterstitial:@"display_failed" params:@{
        @"placement": ad.placement,
        @"error": [self getMAErrorParams:error],
        @"error_code": @(error.code),
        @"ad": [self getMAAdParams:ad]
    }];
    
    self.m_showing = NO;
    
    id<AppleAdvertisementCallbackInterface> callback = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementInterstitialCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementShowFailed:ad.placement withError:error.code];
    }
    
    [self loadAd];
}

#pragma mark - MAAdRevenueDelegate

- (void)didPayRevenueForAd:(MAAd *) ad {
    [self log:@"didPayRevenueForAd" withMAAd:ad];

    [self eventRevenue:ad];
    
    id<AppleAdvertisementCallbackInterface> callback = [[AppleAppLovinApplicationDelegate sharedInstance] getAdvertisementInterstitialCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementRevenuePaid:ad.placement withRevenue:ad.revenue];
    }
}

#pragma mark - MAAdExpirationDelegate

- (void)didReloadExpiredAd:(MAAd *)expiredAd withNewAd:(MAAd *)newAd {
    [self log:@"didReloadExpiredAd" withMAAd:expiredAd];
    
    [self eventInterstitial:@"expired" params:@{
        @"placement": expiredAd.placement,
        @"ad": [self getMAAdParams:expiredAd],
        @"new_ad": [self getMAAdParams:newAd]
    }];
}

#pragma mark - MAAdReviewDelegate

- (void)didGenerateCreativeIdentifier:(NSString *)creativeIdentifier forAd:(MAAd *)ad {
    //ToDo
}

@end
