#import "AppleAppLovinRewardedAmazonLoader.h"

#import <DTBiOSSDK/DTBAdLoader.h>

@implementation AppleAppLovinRewardedAmazonLoader

- (instancetype _Nonnull) initWithSlotId:(NSString * _Nonnull) amazonAdSlotId rewardedAd:(MARewardedAd *_Nonnull) rewardedAd {
    self = [super init];
    
    self.m_rewardedAd = rewardedAd;
    
    DTBAdLoader *adLoader = [[DTBAdLoader alloc] init];
        
    // Switch video player width and height values(320, 480) depending on device orientation
    [adLoader setAdSizes: @[
        [[DTBAdSize alloc] initVideoAdSizeWithPlayerWidth: UIScreen.mainScreen.bounds.size.width
                                                   height: UIScreen.mainScreen.bounds.size.height
                                              andSlotUUID: amazonAdSlotId]
    ]];

    [adLoader loadAd: self];
    
    return self;
}

#pragma mark - DTBAdCallback Protocol

- (void) onFailure:(DTBAdError) error dtbAdErrorInfo:(DTBAdErrorInfo *) errorInfo {
    [self.m_rewardedAd setLocalExtraParameterForKey: @"amazon_ad_error" value: errorInfo];
    [self.m_rewardedAd loadAd];
}

- (void) onSuccess:(DTBAdResponse *) adResponse {
    [self.m_rewardedAd setLocalExtraParameterForKey: @"amazon_ad_response" value: adResponse];
    [self.m_rewardedAd loadAd];
}

@end
