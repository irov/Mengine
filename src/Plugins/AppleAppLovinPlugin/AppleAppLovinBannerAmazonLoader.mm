#import "AppleAppLovinBannerAmazonLoader.h"

#import <DTBiOSSDK/DTBAdLoader.h>

@implementation AppleAppLovinBannerAmazonLoader

- (instancetype _Nonnull) initWithSlotId:(NSString * _Nullable) amazonAdSlotId adView:(MAAdView * _Nonnull) adView rect:(CGRect) rect {
    self = [super init];
    
    self.m_adView = adView;
    
    DTBAdSize *size = [[DTBAdSize alloc] initBannerAdSizeWithWidth: rect.size.width
                                                            height: rect.size.height
                                                       andSlotUUID: amazonAdSlotId];
        
    DTBAdLoader *adLoader = [[DTBAdLoader alloc] init];
    [adLoader setAdSizes: @[size]];
    [adLoader loadAd: self];
    
    return self;
}

#pragma mark - DTBAdCallback Protocol

- (void) onFailure:(DTBAdError) error dtbAdErrorInfo:(DTBAdErrorInfo *) errorInfo {
    [self.m_adView setLocalExtraParameterForKey: @"amazon_ad_error" value: errorInfo];
    [self.m_adView loadAd];
}

- (void) onSuccess:(DTBAdResponse *) adResponse{
    [self.m_adView setLocalExtraParameterForKey: @"amazon_ad_response" value: adResponse];
    [self.m_adView loadAd];
}

@end
