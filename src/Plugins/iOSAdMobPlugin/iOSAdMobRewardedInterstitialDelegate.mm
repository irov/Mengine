#import "iOSAdMobRewardedInterstitialDelegate.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSNetwork.h"

@implementation iOSAdMobRewardedInterstitialDelegate

- (instancetype)initWithAdUnitIdentifier:(NSString *)adUnitId advertisement:(id<iOSAdvertisementInterface>)advertisement {
    self = [super initWithAdUnitIdentifier:adUnitId adFormat:@"rewarded_interstitial" advertisement:advertisement];

    if (self != nil) {
        self.m_loading = NO;
        self.m_showing = NO;
        self.m_rewarded = NO;
        self.m_loadedTimestamp = 0.0;

        [self loadAd];
    }

    return self;
}

- (void)dealloc {
    self.m_ad.fullScreenContentDelegate = nil;
    self.m_ad.paidEventHandler = nil;
}

- (void)eventRewardedInterstitial:(NSString *)name params:(NSDictionary *)params {
    NSString * eventName = [@"mng_admob_rewarded_interstitial_" stringByAppendingString:name];
    [self event:eventName params:params];
}

- (BOOL)canYouShow:(NSString *)placement {
    if (self.m_showing == NO) {
        if (self.m_ad != nil) {
            if (NSProcessInfo.processInfo.systemUptime - self.m_loadedTimestamp >= 3600.0) {
                [self log:@"rewardedInterstitial.expired"];
                self.m_ad.fullScreenContentDelegate = nil;
                self.m_ad.paidEventHandler = nil;
                self.m_ad = nil;

                [self loadAd];
            }
        }
    }

    BOOL ready = NO;

    if (self.m_ad != nil) {
        if (self.m_showing == NO) {
            ready = [[iOSNetwork sharedInstance] isNetworkAvailable];
        }
    }

    [self log:@"rewardedInterstitial.canYouShow" withParams:@{@"placement":placement, @"ready":@(ready), @"loading":@(self.m_loading)}];

    return ready;
}

- (BOOL)show:(NSString *)placement {
    if ([self canYouShow:placement] == NO) {
        return NO;
    }

    UIViewController * viewController = [iOSDetail getRootViewController];

    if (viewController == nil) {
        [self log:@"rewardedInterstitial.rootViewControllerMissing"];
        return NO;
    }

    self.m_placement = placement;
    self.m_showing = YES;
    self.m_rewarded = NO;

    GADAdReward * reward = self.m_ad.adReward;
    __weak GADRewardedInterstitialAd * weakAd = self.m_ad;
    __weak iOSAdMobRewardedInterstitialDelegate * weakSelf = self;

    [self log:@"rewardedInterstitial.show" withParams:@{@"placement":placement}];

    [self.m_ad presentFromRootViewController:viewController userDidEarnRewardHandler:^{
        iOSAdMobRewardedInterstitialDelegate * strongSelf = weakSelf;

        if (strongSelf == nil) {
            return;
        }

        if (strongSelf.m_showing == NO) {
            return;
        }

        if (strongSelf.m_ad != weakAd) {
            return;
        }

        if (strongSelf.m_rewarded == YES) {
            return;
        }

        strongSelf.m_rewarded = YES;

        [strongSelf log:@"rewardedInterstitial.userDidEarnReward"];
        [strongSelf eventRewardedInterstitial:@"rewarded" params:@{@"placement":placement}];

        id<iOSAdvertisementCallbackInterface> callback = [strongSelf.m_advertisement getRewardedCallback];
        [callback oniOSAdvertisementUserRewarded:placement withLabel:reward.type != nil ? reward.type : @"" withAmount:reward.amount.integerValue];
    }];

    return YES;
}

- (BOOL)isShowing {
    return self.m_showing;
}

- (void)loadAd {
    if (self.m_loading == YES) {
        return;
    }

    if (self.m_showing == YES) {
        return;
    }

    if (self.m_ad != nil) {
        return;
    }

    self.m_loading = YES;

    [self increaseRequestId];
    [self log:@"rewardedInterstitial.load"];
    [self eventRewardedInterstitial:@"load" params:@{}];

    GADRequest * request = [self createAdRequest];

    __weak iOSAdMobRewardedInterstitialDelegate * weakSelf = self;

    [GADRewardedInterstitialAd loadWithAdUnitID:self.m_adUnitId request:request
        completionHandler:^(GADRewardedInterstitialAd * ad, NSError * error) {
        iOSAdMobRewardedInterstitialDelegate * strongSelf = weakSelf;

        if (strongSelf == nil) {
            return;
        }

        strongSelf.m_loading = NO;

        if (error != nil) {
            [strongSelf log:@"rewardedInterstitial.loadFailed" withError:error];
            NSDictionary * errorParams = [strongSelf getGADAdErrorParams:error];
            [strongSelf eventRewardedInterstitial:@"load_failed" params:@{@"error":errorParams, @"domain":error.domain}];

            [strongSelf retryLoadAd];
            return;
        }

        if (ad == nil) {
            [strongSelf log:@"rewardedInterstitial.loadEmpty"];

            [strongSelf retryLoadAd];
            return;
        }

        strongSelf.m_ad = ad;
        strongSelf.m_loadedTimestamp = NSProcessInfo.processInfo.systemUptime;
        strongSelf.m_requestAttempt = 0;
        ad.fullScreenContentDelegate = strongSelf;

        GADResponseInfo * responseInfo = ad.responseInfo;
        ad.paidEventHandler = ^(GADAdValue * value) {
            iOSAdMobRewardedInterstitialDelegate * paidSelf = weakSelf;

            if (paidSelf == nil) {
                return;
            }

            if (paidSelf.m_showing == NO) {
                return;
            }

            [paidSelf eventRevenue:value responseInfo:responseInfo placement:paidSelf.m_placement format:@"ADFORMAT_REWARDED_INTERSTITIAL"];
            id<iOSAdvertisementCallbackInterface> callback = [paidSelf.m_advertisement getRewardedCallback];
            [callback oniOSAdvertisementRevenuePaid:paidSelf.m_placement withRevenue:value.value.doubleValue];
        };

        NSDictionary * responseParams = [strongSelf getGADResponseInfoParams:responseInfo];
        [strongSelf log:@"rewardedInterstitial.loaded" withParams:responseParams];
        [strongSelf eventRewardedInterstitial:@"loaded" params:@{}];
    }];
}

- (void)finishAdWithError:(NSError *)error {
    NSString * placement = self.m_placement;

    [self setAdFreeze:NO];

    self.m_ad.fullScreenContentDelegate = nil;
    self.m_ad.paidEventHandler = nil;
    self.m_ad = nil;
    self.m_placement = nil;
    self.m_showing = NO;

    id<iOSAdvertisementCallbackInterface> callback = [self.m_advertisement getRewardedCallback];

    if (error != nil) {
        [callback oniOSAdvertisementShowFailed:placement withError:error.code];
    } else {
        [callback oniOSAdvertisementShowSuccess:placement];
    }

    [self loadAd];
}

#pragma mark - GADFullScreenContentDelegate

- (void)adWillPresentFullScreenContent:(id<GADFullScreenPresentingAd>)ad {
    [self log:@"rewardedInterstitial.willPresent"];
    [self setAdFreeze:YES];
}

- (void)ad:(id<GADFullScreenPresentingAd>)ad didFailToPresentFullScreenContentWithError:(NSError *)error {
    [self log:@"rewardedInterstitial.presentFailed" withError:error];
    [self finishAdWithError:error];
}

- (void)adDidDismissFullScreenContent:(id<GADFullScreenPresentingAd>)ad {
    [self log:@"rewardedInterstitial.dismissed"];
    [self finishAdWithError:nil];
}

- (void)adDidRecordImpression:(id<GADFullScreenPresentingAd>)ad {
    [self eventRewardedInterstitial:@"impression" params:@{}];
}

- (void)adDidRecordClick:(id<GADFullScreenPresentingAd>)ad {
    [self eventRewardedInterstitial:@"clicked" params:@{}];
}

@end
