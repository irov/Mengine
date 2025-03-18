#import "AppleAdvertisementApplicationDelegate.h"

#import "Environment/iOS/iOSDetail.h"

@implementation AppleAdvertisementApplicationDelegate

#pragma mark - AppleAdvertisementInterface

+ (instancetype) sharedInstance {
    static AppleAdvertisementApplicationDelegate * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleAdvertisementApplicationDelegate class]];
    });
    return sharedInstance;
}

- (void)setProvider:(id<AppleAdvertisementProviderInterface>)provider {
    self.m_provider = provider;
}

- (id<AppleAdvertisementProviderInterface>)getProvider {
    return self.m_provider;
}

- (void)setBannerCallback:(id<AppleAdvertisementCallbackInterface>)callback {
    self.m_bannerCallback = callback;
}

- (id<AppleAdvertisementCallbackInterface>)getBannerCallback {
    return self.m_bannerCallback;
}

- (void)setInterstitialCallback:(id<AppleAdvertisementCallbackInterface>)callback {
    self.m_interstitialCallback = callback;
}

- (id<AppleAdvertisementCallbackInterface>)getInterstitialCallback {
    return self.m_interstitialCallback;
}

- (void)setRewardedCallback:(id<AppleAdvertisementCallbackInterface>)callback {
    self.m_rewardedCallback = callback;
}

- (id<AppleAdvertisementCallbackInterface>)getRewardedCallback {
    return self.m_rewardedCallback;
}

#pragma mark - AppleAdvertisementProviderInterface

- (BOOL)hasBanner {
    return [self.m_provider hasBanner];
}

- (BOOL)showBanner {
    return [self.m_provider showBanner];
}

- (BOOL)hideBanner {
    return [self.m_provider hideBanner];
}

- (BOOL)getBannerWidth:(uint32_t *)width height:(uint32_t *)height {
    return [self.m_provider getBannerWidth:width height:height];
}

- (BOOL)hasInterstitial {
    return [self.m_provider hasInterstitial];
}

- (BOOL)canYouShowInterstitial:(NSString *)placement {
    return [self.m_provider canYouShowInterstitial:placement];
}

- (BOOL)showInterstitial:(NSString *)placement {
    return [self.m_provider showInterstitial:placement];
}

- (BOOL)hasRewarded {
    return [self.m_provider hasRewarded];
}

- (BOOL)canOfferRewarded:(NSString *)placement {
    return [self.m_provider canOfferRewarded:placement];
}

- (BOOL)canYouShowRewarded:(NSString *)placement {
    return [self.m_provider canYouShowRewarded:placement];
}

- (BOOL)showRewarded:(NSString *)placement {
    return [self.m_provider showRewarded:placement];
}

#pragma mark - iOSPluginConfigDelegateInterface

- (void)onConfig:(NSDictionary * _Nonnull)config {
    
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    return YES;
}

@end
