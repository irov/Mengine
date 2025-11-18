#import "AppleAdMobBannerDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleString.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#import "AppleAdMobApplicationDelegate.h"

@implementation AppleAdMobBannerDelegate

- (instancetype _Nullable) initWithAdUnitIdentifier:(NSString * _Nonnull)adUnitId
                                      advertisement:(id<AppleAdvertisementInterface> _Nonnull)advertisement
                                          placement:(NSString * _Nonnull)placement
                                           adaptive:(BOOL)adaptive {
    self = [super initWithAdUnitIdentifier:adUnitId advertisement:advertisement];
    
    self.m_bannerAdaptive = adaptive;
    
    GADAdSize adSize;
    if (adaptive == YES) {
        CGFloat screen_width = CGRectGetWidth(UIScreen.mainScreen.bounds);
        adSize = GADCurrentOrientationAnchoredAdaptiveBannerAdSizeWithWidth(screen_width);
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
        IOS_LOGGER_ERROR(@"[Error] AppleAdMobBannerDelegate invalid create GADBannerView adUnitId: %@ exception: %@ [%@]"
            , adUnitId
            , ex.reason
            , ex.name
        );
        
        return nil;
    }
    
    bannerView.delegate = self;
    
    CGSize size = adSize.size;    
    
    CGFloat banner_height = size.height;
    
    CGFloat screen_width = CGRectGetWidth(UIScreen.mainScreen.bounds);
    CGFloat screen_height = CGRectGetHeight(UIScreen.mainScreen.bounds);
    
    CGRect rect = CGRectMake(0, screen_height - banner_height, screen_width, banner_height);
    
    bannerView.frame = rect;
    
    bannerView.backgroundColor = UIColor.clearColor;
    
    UIViewController * viewController = [iOSDetail getRootViewController];
    [viewController.view addSubview:bannerView];
    bannerView.hidden = YES;
    
    self.m_bannerView = bannerView;
    
    [self loadAd];
    
    return self;
}

- (void) dealloc {
    if (self.m_bannerView != nil) {
        self.m_bannerView.delegate = nil;
        
        [self.m_bannerView removeFromSuperview];
        self.m_bannerView = nil;
    }
}

- (void) eventBanner:(NSString * _Nonnull) eventName params:(NSDictionary<NSString *, id> * _Nullable) params {
    [self event:[@"mng_admob_banner_" stringByAppendingString:eventName] params:params];
}

- (void) show {
    self.m_bannerView.hidden = NO;
}

- (void) hide {
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
    
    return banner_size.height;
}

- (CGFloat) getWidthPx {
    CGSize banner_size = [self getSize];
    
    return banner_size.width;
}

#pragma mark - GADBannerViewDelegate

- (void)bannerViewDidReceiveAd:(GADBannerView *)bannerView {
    [self log:@"bannerViewDidReceiveAd" withParams:[self getGADResponseInfoParams:bannerView.responseInfo]];
    
    [self eventBanner:@"loaded" params:@{
        @"response": [self getGADResponseInfoParams:bannerView.responseInfo]
    }];
    
    self.m_requestAttempt = 0;
}

- (void)bannerView:(GADBannerView *)bannerView didFailToReceiveAdWithError:(NSError *)error {
    [self log:@"bannerView:didFailToReceiveAdWithError" withError:error];
    
    [self eventBanner:@"load_failed" params:@{
        @"error": [self getGADAdErrorParams:error],
        @"error_code": @(error.code)
    }];
    
    [self retryLoadAd];
}

- (void)bannerViewDidRecordImpression:(GADBannerView *)bannerView {
    [self log:@"bannerViewDidRecordImpression"];
    
    [self eventBanner:@"impression" params:@{}];
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

@end

