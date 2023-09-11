#import "AppleAppLovinBannerDelegate.h"

#include "Environment/Apple/AppleString.h"
#include "Environment/iOS/iOSDetail.h"

#include "Kernel/Logger.h"

@implementation AppleAppLovinBannerDelegate

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitId
                                         placement:(NSString * _Nonnull) placement
                                      amazonSlotId:(NSString * _Nullable) amazonSlotId
                                              rect:(CGRect) rect
                                          provider:(const Mengine::AppleAppLovinBannerProviderInterfacePtr &) provider
                                         analytics:(AppleAppLovinAnalyticsService * _Nonnull) analytics {
    self = [super initWithAdUnitIdentifier:adUnitId analytics:analytics];
    
    self.m_provider = provider;
    
    MAAdView * adView = [[MAAdView alloc] initWithAdUnitIdentifier:adUnitId];
    [adView setPlacement:placement];
    adView.delegate = self;
    adView.revenueDelegate = self;
    adView.requestDelegate = self;

    adView.frame = rect;
    adView.backgroundColor = UIColor.blackColor;

    UIViewController * rootViewController = Mengine::Helper::iOSGetRootViewController();
    [rootViewController.view addSubview:adView];
    
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
    if( self.m_amazonLoader != nil )
    {
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
    [self.m_adView loadAd];
}

#pragma mark - MAAdRequestDelegate Protocol

- (void)didStartAdRequestForAdUnitIdentifier:(NSString *)adUnitIdentifier {
    LOGGER_MESSAGE( "banner didStartAdRequestForAdUnitIdentifier: %s"
        , [adUnitIdentifier UTF8String]
    );
    
    self.m_provider->onAppleAppLovinBannerDidStartAdRequestForAdUnitIdentifier();
}

#pragma mark - MAAdDelegate Protocol

- (void) didLoadAd:(MAAd *) ad {
    CGSize adViewSize = ad.size;
    CGFloat widthDp = adViewSize.width;
    CGFloat heightDp = adViewSize.height;

    LOGGER_MESSAGE( "banner didLoadAd" );
    
    self.m_provider->onAppleAppLovinBannerDidLoadAd();
}

- (void) didFailToLoadAdForAdUnitIdentifier:(NSString *) adUnitIdentifier withError:(MAError *) error {
    LOGGER_MESSAGE( "banner didFailToLoadAdForAdUnitIdentifier" );
    
    self.m_provider->onAppleAppLovinBannerDidFailToLoadAdForAdUnitIdentifier();
}

- (void) didClickAd:(MAAd *) ad {
    LOGGER_MESSAGE( "banner didClickAd" );
    
    self.m_provider->onAppleAppLovinBannerDidClickAd();
}

- (void) didFailToDisplayAd:(MAAd *) ad withError:(MAError *) error {
    LOGGER_MESSAGE( "banner didFailToDisplayAd" );
    
    self.m_provider->onAppleAppLovinBannerDidFailToDisplayAd();
}


#pragma mark - MAAdViewAdDelegate Protocol

- (void) didExpandAd:(MAAd *) ad {
    LOGGER_MESSAGE( "banner didExpandAd" );
    
    self.m_provider->onAppleAppLovinBannerDidExpandAd();
}

- (void) didCollapseAd:(MAAd *) ad {
    LOGGER_MESSAGE( "banner didCollapseAd" );
    
    self.m_provider->onAppleAppLovinBannerDidCollapseAd();
}

#pragma mark - Deprecated Callbacks

- (void) didDisplayAd:(MAAd *) ad {
    LOGGER_MESSAGE( "banner didDisplayAd [deprecated]" );
}

- (void) didHideAd:(MAAd *) ad {
    LOGGER_MESSAGE( "banner didHideAd [deprecated]" );
}
    
#pragma mark - Revenue Callbacks

- (void)didPayRevenueForAd:(MAAd *)ad {
    LOGGER_MESSAGE( "banner didPayRevenueForAd" );
    
    self.m_provider->onAppleAppLovinBannerDidPayRevenueForAd();
}

@end
