#import "iOSAdMobBannerDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleString.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#import "iOSAdMobPlugin.h"

@implementation iOSAdMobBannerDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull)adUnitId
                                      advertisement:(id<iOSAdvertisementInterface> _Nonnull)advertisement
                                          placement:(NSString * _Nonnull)placement
                                             anchor:(EiOSAdvertisementBannerAnchor)anchor
                                           adaptive:(BOOL)adaptive {
    self = [super initWithAdUnitIdentifier:adUnitId adFormat:@"banner" advertisement:advertisement];

    if (self == nil) {
        return nil;
    }

    self.m_bannerAdaptive = adaptive;
    self.m_bannerLoaded = NO;
    self.m_bannerShowRequested = NO;
    self.m_placement = placement;

    GADAdSize adSize;
    if (self.m_bannerAdaptive == YES) {
        CGFloat screen_width = CGRectGetWidth(UIScreen.mainScreen.bounds);
        adSize = GADLargeAnchoredAdaptiveBannerAdSizeWithWidth(screen_width);
    } else {
        BOOL isPad = UIDevice.currentDevice.userInterfaceIdiom == UIUserInterfaceIdiomPad;

        if (isPad == YES) {
            adSize = GADAdSizeLeaderboard;
        } else {
            adSize = GADAdSizeBanner;
        }
    }

    GADBannerView * bannerView;

    @try {
        bannerView = [[GADBannerView alloc] initWithAdSize:adSize];
        bannerView.adUnitID = adUnitId;
    } @catch (NSException * ex) {
        IOS_LOGGER_ERROR(@"[Error] iOSAdMobBannerDelegate invalid create GADBannerView adUnitId: %@ exception: %@ [%@]"
            , adUnitId
            , ex.reason
            , ex.name
        );

        return nil;
    }

    UIViewController * viewController = [iOSDetail getRootViewController];

    if (viewController == nil) {
        IOS_LOGGER_ERROR(@"[Error] iOSAdMobBannerDelegate invalid root view controller for adUnitId: %@", adUnitId);

        return nil;
    }

    if (viewController.view == nil) {
        IOS_LOGGER_ERROR(@"[Error] iOSAdMobBannerDelegate invalid root view controller for adUnitId: %@", adUnitId);

        return nil;
    }

    bannerView.delegate = self;
    bannerView.adSizeDelegate = self;

    UIView * gameView = viewController.view;
    UILayoutGuide * safeArea = gameView.safeAreaLayoutGuide;

    bannerView.translatesAutoresizingMaskIntoConstraints = NO;
    bannerView.backgroundColor = UIColor.clearColor;
    bannerView.hidden = YES;

    [gameView addSubview:bannerView];

    NSLayoutConstraint * edgeConstraint;

    switch (anchor) {
    case IOS_ADVERTISEMENT_BANNER_ANCHOR_BOTTOM:
        edgeConstraint = [bannerView.bottomAnchor constraintEqualToAnchor:safeArea.bottomAnchor];
        break;
    case IOS_ADVERTISEMENT_BANNER_ANCHOR_TOP:
        edgeConstraint = [bannerView.topAnchor constraintEqualToAnchor:safeArea.topAnchor];
        break;
    }

    NSLayoutConstraint * centerConstraint = [bannerView.centerXAnchor constraintEqualToAnchor:gameView.centerXAnchor];
    [NSLayoutConstraint activateConstraints:@[centerConstraint, edgeConstraint]];

    self.m_bannerView = bannerView;

    __weak iOSAdMobBannerDelegate * weakSelf = self;
    bannerView.paidEventHandler = ^(GADAdValue * _Nonnull adValue) {
        iOSAdMobBannerDelegate * strongSelf = weakSelf;

        if (strongSelf == nil) {
            return;
        }

        [strongSelf log:@"paidEventHandler" withParams:@{
            @"value": adValue.value,
            @"currencyCode": adValue.currencyCode
        }];

        [strongSelf eventRevenue:adValue responseInfo:strongSelf.m_bannerView.responseInfo placement:strongSelf.m_placement format:@"ADFORMAT_BANNER"];

        id<iOSAdvertisementCallbackInterface> callback = [[iOSAdMobPlugin sharedInstance] getAdvertisementBannerCallback];

        if (callback != nil) {
            [callback oniOSAdvertisementRevenuePaid:strongSelf.m_placement withRevenue:adValue.value.doubleValue];
        }
    };

    [self loadAd];

    return self;
}

- (void) dealloc {
    if (self.m_bannerView != nil) {
        self.m_bannerView.delegate = nil;
        self.m_bannerView.adSizeDelegate = nil;

        [self.m_bannerView removeFromSuperview];
        self.m_bannerView = nil;
    }
}

- (void) eventBanner:(NSString * _Nonnull) eventName params:(NSDictionary<NSString *, id> * _Nullable) params {
    [self event:[@"mng_admob_banner_" stringByAppendingString:eventName] params:params];
}

- (void) show {
    [self log:@"show" withParams:@{@"loaded": @(self.m_bannerLoaded)}];
    self.m_bannerShowRequested = YES;
    self.m_bannerView.hidden = self.m_bannerLoaded == NO;
}

- (void) hide {
    [self log:@"hide"];
    self.m_bannerShowRequested = NO;
    self.m_bannerView.hidden = YES;
}

- (void) loadAd {
    if (self.m_bannerView == nil) {
        return;
    }

    [self increaseRequestId];

    [self log:@"loadAd"];

    [self eventBanner:@"load" params:@{}];

    UIViewController * viewController = [iOSDetail getRootViewController];

    if (viewController == nil) {
        IOS_LOGGER_ERROR(@"[Error] iOSAdMobBannerDelegate invalid root view controller in loadAd for adUnitId: %@", self.m_adUnitId);

        return;
    }

    self.m_bannerView.rootViewController = viewController;

    GADRequest * request = [self createAdRequest];
    [self.m_bannerView loadRequest:request];
}

- (CGSize) getSize {
    if (self.m_bannerView == nil) {
        return CGSizeZero;
    }

    return self.m_bannerView.adSize.size;
}

- (CGRect) getRect {
    if (self.m_bannerView == nil) {
        return CGRectZero;
    }

    CGRect frame = self.m_bannerView.frame;

    return frame;
}

- (CGFloat) getHeightPx {
    CGSize banner_size = [self getSize];

    CGFloat banner_scale = UIScreen.mainScreen.scale;

    return banner_size.height * banner_scale;
}

- (CGFloat) getWidthPx {
    CGSize banner_size = [self getSize];

    CGFloat banner_scale = UIScreen.mainScreen.scale;

    return banner_size.width * banner_scale;
}

#pragma mark - GADBannerViewDelegate

- (void)bannerViewDidReceiveAd:(GADBannerView *)bannerView {
    self.m_bannerLoaded = YES;
    self.m_bannerView.hidden = self.m_bannerShowRequested == NO;

    NSDictionary<NSString *, id> * responseParams = [self getGADResponseInfoParams:bannerView.responseInfo];

    [self log:@"bannerViewDidReceiveAd" withParams:responseParams];

    [self eventBanner:@"loaded" params:@{
        @"response": responseParams
    }];

    self.m_requestAttempt = 0;
}

- (void)bannerView:(GADBannerView *)bannerView didFailToReceiveAdWithError:(NSError *)error {
    self.m_bannerLoaded = NO;
    self.m_bannerView.hidden = YES;

    [self log:@"bannerView:didFailToReceiveAdWithError" withError:error];

    [self eventBanner:@"load_failed" params:@{
        @"error": [self getGADAdErrorParams:error],
        @"error_code": @(error.code)
    }];

    IOS_LOGGER_WARNING(@"[AdMob] banner unavailable: adUnitId=%@ domain=%@ code=%ld reason=%@; retry scheduled"
        , self.m_adUnitId
        , error.domain
        , (long)error.code
        , error.localizedDescription
    );

    [self retryLoadAd];
}

- (void)bannerViewDidRecordImpression:(GADBannerView *)bannerView {
    [self log:@"bannerViewDidRecordImpression"];

    [self eventBanner:@"impression" params:@{}];
}

- (void)bannerViewDidRecordClick:(GADBannerView *)bannerView {
    [self log:@"bannerViewDidRecordClick"];

    [self eventBanner:@"clicked" params:@{}];
}

- (void)bannerViewWillPresentScreen:(GADBannerView *)bannerView {
    [self log:@"bannerViewWillPresentScreen"];

    [self eventBanner:@"will_present" params:@{}];
}

- (void)bannerViewWillDismissScreen:(GADBannerView *)bannerView {
    [self log:@"bannerViewWillDismissScreen"];

    [self eventBanner:@"will_dismiss" params:@{}];
}

- (void)bannerViewDidDismissScreen:(GADBannerView *)bannerView {
    [self log:@"bannerViewDidDismissScreen"];

    [self eventBanner:@"dismissed" params:@{}];
}

#pragma mark - GADAdSizeDelegate

- (void)adView:(GADBannerView *)bannerView willChangeAdSizeTo:(GADAdSize)size {
    NSDictionary<NSString *, id> * params = @{
        @"width": @(size.size.width),
        @"height": @(size.size.height)
    };

    [self log:@"adView:willChangeAdSizeTo" withParams:params];

    [self eventBanner:@"will_change_size" params:params];
}

@end
