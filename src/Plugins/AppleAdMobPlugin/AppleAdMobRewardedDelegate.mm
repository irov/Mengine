#import "AppleAdMobRewardedDelegate.h"

#include "Interface/PlatformServiceInterface.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleString.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSNetwork.h"

#import "AppleAdMobApplicationDelegate.h"

@implementation AppleAdMobRewardedDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull)adUnitId advertisement:(id<AppleAdvertisementInterface> _Nonnull)advertisement {
    self = [super initWithAdUnitIdentifier:adUnitId advertisement:advertisement];
    
    self.m_rewardedAd = nil;
    
    self.m_showing = NO;
    
    [self loadAd];
    
    return self;
}

- (void) dealloc {
    [self destroyRewardedAd];
}

- (void) destroyRewardedAd {
    if (self.m_rewardedAd != nil) {
        self.m_rewardedAd.fullScreenContentDelegate = nil;
        
        self.m_rewardedAd = nil;
    }
}

- (void) eventRewarded:(NSString * _Nonnull) eventName params:(NSDictionary<NSString *, id> * _Nullable) params {
    [self event:[@"mng_admob_rewarded_" stringByAppendingString:eventName] params:params];
}

- (BOOL) canOffer:(NSString * _Nonnull)placement {
    BOOL ready = (self.m_rewardedAd != nil);
    
    [self log:@"canOffer" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    [self eventRewarded:@"offer" params:@{
        @"placement": placement,
        @"ready": @(ready)
    }];
    
    return ready;
}

- (BOOL) canYouShow:(NSString * _Nonnull)placement {
    BOOL ready = (self.m_rewardedAd != nil);
    
    [self log:@"canYouShow" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    if( ready == NO ) {
        [self eventRewarded:@"show" params:@{
            @"placement": placement,
            @"ready": @(NO)
        }];
        
        return NO;
    }
    
    if ([[iOSNetwork sharedInstance] isNetworkAvailable] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL) show:(NSString * _Nonnull)placement {
    BOOL ready = (self.m_rewardedAd != nil);
    
    [self log:@"show" withParams:@{@"placement":placement, @"ready":@(ready)}];
    
    [self eventRewarded:@"show" params:@{
        @"placement": placement,
        @"ready": @(ready)
    }];
    
    if (ready == NO) {
        return NO;
    }
    
    self.m_showing = YES;
    
    UIViewController * viewController = [iOSDetail getRootViewController];
    
    __weak AppleAdMobRewardedDelegate * weakSelf = self;
    
    [self.m_rewardedAd presentFromRootViewController:viewController
                             userDidEarnRewardHandler:^{
        __strong AppleAdMobRewardedDelegate * strongSelf = weakSelf;
        
        if (strongSelf == nil) {
            return;
        }
        
        GADAdReward * reward = strongSelf.m_rewardedAd.adReward;
        
        [strongSelf log:@"userDidEarnReward"];
        
        [strongSelf eventRewarded:@"rewarded" params:@{
            @"placement": placement,
            @"label": reward.type != nil ? reward.type : @"",
            @"amount": @(reward.amount.doubleValue)
        }];
        
        id<AppleAdvertisementCallbackInterface> callback = [[AppleAdMobApplicationDelegate sharedInstance] getAdvertisementRewardedCallback];
        
        if (callback != nil) {
            NSString * label = reward.type != nil ? reward.type : @"";
            NSInteger amount = (NSInteger)reward.amount.doubleValue;
            [callback onAppleAdvertisementUserRewarded:placement withLabel:label withAmount:amount];
        }
    }];
    
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
    
    [self eventRewarded:@"load" params:@{}];
    
    GADRequest * request = [self createAdRequest];
    
    __weak AppleAdMobRewardedDelegate * weakSelf = self;
    
    [GADRewardedAd loadWithAdUnitID:self.m_adUnitId
                             request:request
                   completionHandler:^(GADRewardedAd * _Nullable rewardedAd, NSError * _Nullable error) {
        __strong AppleAdMobRewardedDelegate * strongSelf = weakSelf;
        
        if (strongSelf == nil) {
            return;
        }
        
        if (error != nil) {
            [strongSelf log:@"loadWithAdUnitID:completionHandler" withError:error];
            
            [strongSelf eventRewarded:@"load_failed" params:@{
                @"error": [strongSelf getGADAdErrorParams:error],
                @"error_code": @(error.code)
            }];
            
            [strongSelf retryLoadAd];
            
            return;
        }
        
        if (rewardedAd == nil) {
            return;
        }
        
        rewardedAd.fullScreenContentDelegate = strongSelf;
        strongSelf.m_rewardedAd = rewardedAd;
        
        [strongSelf log:@"loadWithAdUnitID:completionHandler" withParams:[strongSelf getGADResponseInfoParams:rewardedAd.responseInfo]];
        
        [strongSelf eventRewarded:@"loaded" params:@{
            @"response": [strongSelf getGADResponseInfoParams:rewardedAd.responseInfo]
        }];
        
        strongSelf.m_requestAttempt = 0;
    }];
}

#pragma mark - GADFullScreenContentDelegate

- (void)adWillPresentFullScreenContent:(id<GADFullScreenPresentingAd>)ad {
    [self log:@"adWillPresentFullScreenContent"];
    
    [self eventRewarded:@"will_present" params:@{}];
    
    PLATFORM_SERVICE()
        ->freezePlatform( true, true, true );
}

- (void)ad:(id<GADFullScreenPresentingAd>)ad didFailToPresentFullScreenContentWithError:(NSError *)error {
    [self log:@"ad:didFailToPresentFullScreenContentWithError" withError:error];
        
    [self eventRewarded:@"present_failed" params:@{
        @"error": [self getGADAdErrorParams:error],
        @"error_code": @(error.code)
    }];
    
    [self destroyRewardedAd];
    
    self.m_showing = NO;
    
    id<AppleAdvertisementCallbackInterface> callback = [[AppleAdMobApplicationDelegate sharedInstance] getAdvertisementRewardedCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementShowFailed:@"unknown" withError:error.code];
    }
    
    [self loadAd];
}

- (void)adDidDismissFullScreenContent:(id<GADFullScreenPresentingAd>)ad {
    [self log:@"adDidDismissFullScreenContent"];
    
    [self eventRewarded:@"dismissed" params:@{}];
    
    PLATFORM_SERVICE()
        ->unfreezePlatform( true, true, true );
    
    [self destroyRewardedAd];
    
    self.m_showing = NO;
    
    id<AppleAdvertisementCallbackInterface> callback = [[AppleAdMobApplicationDelegate sharedInstance] getAdvertisementRewardedCallback];
    
    if (callback != nil) {
        [callback onAppleAdvertisementShowSuccess:@"unknown"];
    }
    
    [self loadAd];
}

- (void)adDidRecordImpression:(id<GADFullScreenPresentingAd>)ad {
    [self log:@"adDidRecordImpression"];
    
    [self eventRewarded:@"impression" params:@{}];
}

- (void)adDidRecordClick:(id<GADFullScreenPresentingAd>)ad {
    [self log:@"adDidRecordClick"];
    
    [self eventRewarded:@"clicked" params:@{}];
}

@end

