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

- (void)setAdvertisementProvider:(id<AppleAdvertisementProviderInterface>)provider {
    self.m_provider = provider;
}

- (id<AppleAdvertisementProviderInterface>)getAdvertisementProvider {
    return self.m_provider;
}

- (void)setAdvertisementCallback:(id<AppleAdvertisementCallbackInterface>)response {
    self.m_response = response;
}

- (id<AppleAdvertisementCallbackInterface>)getAdvertisementCallback {
    return self.m_response;
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

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    return YES;
}

@end
