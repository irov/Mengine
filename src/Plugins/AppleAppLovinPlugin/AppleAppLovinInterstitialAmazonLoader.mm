#import "AppleAppLovinInterstitialAmazonLoader.h"

@implementation AppleAppLovinInterstitialAmazonLoader

- (instancetype _Nonnull) initWithSlotId:(NSString* _Nullable) amazonAdSlotId interstitialAd:(MAInterstitialAd* _Nonnull) interstitialAd {
    self = [super init];
    
    self.m_interstitialAd = interstitialAd;
    
    DTBAdLoader *adLoader = [[DTBAdLoader alloc] init];
    [adLoader setAdSizes: @[
        [[DTBAdSize alloc] initInterstitialAdSizeWithSlotUUID: amazonAdSlotId]
    ]];

    [adLoader loadAd: self];
    
    return self;
}

#pragma mark - DTBAdCallback Protocol

- (void)onFailure:(DTBAdError) error dtbAdErrorInfo:(DTBAdErrorInfo *) errorInfo {
    [self.m_interstitialAd setLocalExtraParameterForKey: @"amazon_ad_error" value: errorInfo];
    [self.m_interstitialAd loadAd];
}

- (void)onSuccess:(DTBAdResponse *) adResponse {
    [self.m_interstitialAd setLocalExtraParameterForKey: @"amazon_ad_response" value: adResponse];
    [self.m_interstitialAd loadAd];
}

@end
