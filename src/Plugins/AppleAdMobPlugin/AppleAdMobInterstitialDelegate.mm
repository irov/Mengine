#import "AppleAdMobInterstitialDelegate.h"

#include "Interface/PlatformServiceInterface.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleString.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#import "AppleAdMobApplicationDelegate.h"

@implementation AppleAdMobInterstitialDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull)adUnitId advertisement:(id<AppleAdvertisementInterface> _Nonnull)advertisement {
    self = [super initWithAdUnitIdentifier:adUnitId advertisement:advertisement];
    
    self.m_interstitialAd = nil;
    
    self.m_showing = NO;
    
    [self loadAd];
    
    return self;
}

- (void) dealloc {
    if (self.m_interstitialAd != nil) {
        self.m_interstitialAd.fullScreenContentDelegate = nil;
        
        self.m_interstitialAd = nil;
    }
}

- (void) eventInterstitial:(NSString * _Nonnull) eventName params:(NSDictionary<NSString *, id> * _Nullable) params {
    [self event:[@"mng_admob_interstitial_" stringByAppendingString:eventName] params:params];
}

- (BOOL) canYouShow:(NSString * _Nonnull)placement {
    BOOL ready = (self.m_interstitialAd != nil);
    
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
    BOOL ready = (self.m_interstitialAd != nil);
       
    [self log:@"show" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    [self eventInterstitial:@"show" params:@{
        @"placement": placement,
        @"ready": @(ready)
    }];
    
    if (ready == NO) {
        return NO;
    }
    
    self.m_showing = YES;
    
    UIViewController * viewController = [iOSDetail getRootViewController];
    [self.m_interstitialAd presentFromRootViewController:viewController];
    
    return YES;
}

- (BOOL) isShowing {
    return self.m_showing;
}

- (void) loadAd {
    if (self.m_adUnitId == nil) {
        return;
    }
    
    [self increaseRequestId];
    
    [self log:@"loadAd"];
    
    [self eventInterstitial:@"load" params:@{}];
    
    GADRequest * request = [GADRequest request];
    
    __weak AppleAdMobInterstitialDelegate * weakSelf = self;
    
    [GADInterstitialAd loadWithAdUnitID:self.m_adUnitId
                                request:request
                      completionHandler:^(GADInterstitialAd * _Nullable interstitialAd, NSError * _Nullable error) {
        __strong AppleAdMobInterstitialDelegate * strongSelf = weakSelf;
        
        if (strongSelf == nil) {
            return;
        }
        
        if (error != nil) {
            [strongSelf log:@"loadWithAdUnitID:completionHandler" withError:error];
            
            [strongSelf eventInterstitial:@"load_failed" params:@{
                @"error": [strongSelf getGADAdErrorParams:error],
                @"error_code": @(error.code)
            }];
            
            [strongSelf retryLoadAd];
            
            return;
        }
        
        if (interstitialAd == nil) {
            return;
        }
        
        interstitialAd.fullScreenContentDelegate = strongSelf;
        strongSelf.m_interstitialAd = interstitialAd;
        
        [strongSelf log:@"loadWithAdUnitID:completionHandler" withParams:[strongSelf getGADResponseInfoParams:interstitialAd.responseInfo]];
        
        [strongSelf eventInterstitial:@"loaded" params:@{
            @"response": [strongSelf getGADResponseInfoParams:interstitialAd.responseInfo]
        }];
        
        strongSelf.m_requestAttempt = 0;
    }];
}

#pragma mark - GADFullScreenContentDelegate

- (void)adWillPresentFullScreenContent:(id<GADFullScreenPresentingAd>)ad {
    [self log:@"adWillPresentFullScreenContent"];
    
    [self eventInterstitial:@"will_present" params:@{}];
    
    PLATFORM_SERVICE()
        ->freezePlatform( true, true, true );
}

- (void)ad:(id<GADFullScreenPresentingAd>)ad didFailToPresentFullScreenContentWithError:(NSError *)error {
    [self log:@"ad:didFailToPresentFullScreenContentWithError" withError:error];
        
    [self eventInterstitial:@"present_failed" params:@{
        @"error": [self getGADAdErrorParams:error],
        @"error_code": @(error.code)
    }];
    
    self.m_showing = NO;
    
    id<AppleAdvertisementCallbackInterface> callback = [[AppleAdMobApplicationDelegate sharedInstance] getAdvertisementInterstitialCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementShowFailed:@"unknown" withError:error.code];
    }
    
    [self loadAd];
}

- (void)adDidDismissFullScreenContent:(id<GADFullScreenPresentingAd>)ad {
    [self log:@"adDidDismissFullScreenContent"];
    
    [self eventInterstitial:@"dismissed" params:@{}];
    
    PLATFORM_SERVICE()
        ->unfreezePlatform( true, true, true );
    
    self.m_showing = NO;
    
    id<AppleAdvertisementCallbackInterface> callback = [[AppleAdMobApplicationDelegate sharedInstance] getAdvertisementInterstitialCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementShowSuccess:@"unknown"];
    }
    
    self.m_interstitialAd = nil;
    [self loadAd];
}

- (void)adDidRecordImpression:(id<GADFullScreenPresentingAd>)ad {
    [self log:@"adDidRecordImpression"];
    
    [self eventInterstitial:@"impression" params:@{}];
}

- (void)adDidRecordClick:(id<GADFullScreenPresentingAd>)ad {
    [self log:@"adDidRecordClick"];
    
    [self eventInterstitial:@"clicked" params:@{}];
}

@end

