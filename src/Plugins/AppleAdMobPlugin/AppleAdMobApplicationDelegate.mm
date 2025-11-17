#import "AppleAdMobApplicationDelegate.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleSemaphoreService.h"
#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSNetwork.h"
#import "Environment/iOS/iOSLog.h"

#import "Plugins/AppleAdvertisementPlugin/AppleAdvertisementInterface.h"

#include "Config/Version.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_DEBUG)
#   import <AdSupport/AdSupport.h>
#endif

@implementation AppleAdMobApplicationDelegate

- (instancetype)init {
    self = [super init];
    
    if (self) {
        self.m_bannerAd = nil;
        self.m_interstitialAd = nil;
        self.m_rewardedAd = nil;
    }
    
    return self;
}

- (id<AppleAdvertisementCallbackInterface>)getAdvertisementBannerCallback {
    id<AppleAdvertisementInterface> advertisement = [iOSDetail getPluginDelegateOfProtocol:@protocol(AppleAdvertisementInterface)];
    
    id<AppleAdvertisementCallbackInterface> callback = [advertisement getBannerCallback];
    
    return callback;
}

- (id<AppleAdvertisementCallbackInterface>)getAdvertisementInterstitialCallback {
    id<AppleAdvertisementInterface> advertisement = [iOSDetail getPluginDelegateOfProtocol:@protocol(AppleAdvertisementInterface)];
    
    id<AppleAdvertisementCallbackInterface> callback = [advertisement getInterstitialCallback];
    
    return callback;
}

- (id<AppleAdvertisementCallbackInterface>)getAdvertisementRewardedCallback {
    id<AppleAdvertisementInterface> advertisement = [iOSDetail getPluginDelegateOfProtocol:@protocol(AppleAdvertisementInterface)];
    
    id<AppleAdvertisementCallbackInterface> callback = [advertisement getRewardedCallback];
    
    return callback;
}

- (AppleAdMobBannerDelegate *)getBanner {
    return self.m_bannerAd;
}

- (AppleAdMobInterstitialDelegate *)getInterstitial {
    return self.m_interstitialAd;
}

- (AppleAdMobRewardedDelegate *)getRewarded {
    return self.m_rewardedAd;
}

#pragma mark - AppleAdMobInterface

+ (instancetype)sharedInstance {
    static AppleAdMobApplicationDelegate * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleAdMobApplicationDelegate class]];
    });
    return sharedInstance;
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if ([AppleBundle hasPluginConfig:@PLUGIN_BUNDLE_NAME] == NO) {
        IOS_LOGGER_ERROR(@"[ERROR] AdMob plugin not found bundle config [%@]", @PLUGIN_BUNDLE_NAME);
        
        return NO;
    }
    
    GADMobileAds * mobileAds = [GADMobileAds sharedInstance];
    
    GADVersionNumber version = [GADMobileAds versionNumber];
    NSString * versionString = [NSString stringWithFormat:@"%lu.%lu.%lu", version.majorVersion, version.minorVersion, version.patchVersion];
    
    IOS_LOGGER_MESSAGE(@"AdMob: %@", versionString);
    
    NSString * MengineAppleAdMobPlugin_AppId = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"AppId" withDefault:nil];
    
    if (MengineAppleAdMobPlugin_AppId != nil && [MengineAppleAdMobPlugin_AppId length] > 0) {
        [GADMobileAds configureWithApplicationID:MengineAppleAdMobPlugin_AppId];
        
        IOS_LOGGER_MESSAGE(@"[AdMob] configure with AppId: %@", MengineAppleAdMobPlugin_AppId);
    }
    
    BOOL MengineAppleAdMobPlugin_RequestConfigurationTestDeviceIdsEnabled = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"RequestConfigurationTestDeviceIdsEnabled" withDefault:NO];
    
    if (MengineAppleAdMobPlugin_RequestConfigurationTestDeviceIdsEnabled == YES) {
        GADRequestConfiguration * requestConfiguration = mobileAds.requestConfiguration;
        
#if defined(MENGINE_DEBUG)
        if ([AppleDetail hasOption:@"admob.test_device_advertising"] == YES) {
            NSString * testDeviceId = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
            requestConfiguration.testDeviceIdentifiers = @[testDeviceId];
            
            IOS_LOGGER_MESSAGE(@"[AdMob] test device id: %@", testDeviceId);
        }
#endif
    }
    
    id<AppleAdvertisementInterface> advertisement = [iOSDetail getPluginDelegateOfProtocol:@protocol(AppleAdvertisementInterface)];
    
    [advertisement setProvider:self];
    
    NSString * MengineAppleAdMobPlugin_BannerAdUnitId = nil;
    NSString * MengineAppleAdMobPlugin_InterstitialAdUnitId = nil;
    NSString * MengineAppleAdMobPlugin_RewardedAdUnitId = nil;
    
#if defined(MENGINE_PLUGIN_APPLE_ADMOB_BANNER)
    MengineAppleAdMobPlugin_BannerAdUnitId = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"BannerAdUnitId" withDefault:nil];
#endif
    
#if defined(MENGINE_PLUGIN_APPLE_ADMOB_INTERSTITIAL)
    MengineAppleAdMobPlugin_InterstitialAdUnitId = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"InterstitialAdUnitId" withDefault:nil];
#endif
    
#if defined(MENGINE_PLUGIN_APPLE_ADMOB_REWARDED)
    MengineAppleAdMobPlugin_RewardedAdUnitId = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"RewardedAdUnitId" withDefault:nil];
#endif
    
    __weak AppleAdMobApplicationDelegate * weakSelf = self;
    
    [mobileAds startWithCompletionHandler:^(GADInitializationStatus * _Nonnull status) {
        __strong AppleAdMobApplicationDelegate * strongSelf = weakSelf;
        
        if (strongSelf == nil) {
            return;
        }
        
        IOS_LOGGER_MESSAGE(@"[AdMob] plugin initialize complete");
        
        id<AppleAdvertisementInterface> advertisement = [iOSDetail getPluginDelegateOfProtocol:@protocol(AppleAdvertisementInterface)];
        
#if defined(MENGINE_PLUGIN_APPLE_ADMOB_BANNER)
        if (MengineAppleAdMobPlugin_BannerAdUnitId != nil) {
            NSString * MengineAppleAdMobPlugin_BannerPlacement = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"BannerPlacement" withDefault:@"banner"];
            
            BOOL MengineAppleAdMobPlugin_BannerAdaptive = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"BannerAdaptive" withDefault:YES];
            
            AppleAdMobBannerDelegate * bannerAd = [[AppleAdMobBannerDelegate alloc] initWithAdUnitIdentifier:MengineAppleAdMobPlugin_BannerAdUnitId advertisement:advertisement placement:MengineAppleAdMobPlugin_BannerPlacement adaptive:MengineAppleAdMobPlugin_BannerAdaptive];
            
            strongSelf.m_bannerAd = bannerAd;
        }
#endif
        
#if defined(MENGINE_PLUGIN_APPLE_ADMOB_INTERSTITIAL)
        if (MengineAppleAdMobPlugin_InterstitialAdUnitId != nil) {
            AppleAdMobInterstitialDelegate * interstitialAd = [[AppleAdMobInterstitialDelegate alloc] initWithAdUnitIdentifier:MengineAppleAdMobPlugin_InterstitialAdUnitId advertisement:advertisement];
            
            strongSelf.m_interstitialAd = interstitialAd;
        }
#endif
        
#if defined(MENGINE_PLUGIN_APPLE_ADMOB_REWARDED)
        if (MengineAppleAdMobPlugin_RewardedAdUnitId != nil) {
            AppleAdMobRewardedDelegate * rewardedAd = [[AppleAdMobRewardedDelegate alloc] initWithAdUnitIdentifier:MengineAppleAdMobPlugin_RewardedAdUnitId advertisement:advertisement];
            
            strongSelf.m_rewardedAd = rewardedAd;
        }
#endif
        
        [advertisement readyAdProvider];
    }];
    
    return YES;
}

#pragma mark - AppleAdvertisementProviderInterface

- (BOOL)hasBanner {
    if (self.m_bannerAd == nil) {
        return NO;
    }
    
    return YES;
}

- (BOOL)showBanner {
    if (self.m_bannerAd == nil) {
        return NO;
    }
    
    [self.m_bannerAd show];
    
    return YES;
}

- (BOOL)hideBanner {
    if (self.m_bannerAd == nil) {
        return NO;
    }
    
    [self.m_bannerAd hide];
    
    return YES;
}

- (BOOL)getBannerWidth:(uint32_t *)width height:(uint32_t *)height {
    if (self.m_bannerAd == nil) {
        return NO;
    }
    
    *width = [self.m_bannerAd getWidthPx];
    *height = [self.m_bannerAd getHeightPx];
    
    return YES;
}

- (BOOL)hasInterstitial {
    if (self.m_interstitialAd == nil) {
        return NO;
    }
    
    return YES;
}

- (BOOL)canYouShowInterstitial:(NSString *)placement {
    if (self.m_interstitialAd == nil) {
        return NO;
    }
    
    if ([[iOSNetwork sharedInstance] isNetworkAvailable] == NO) {
        return NO;
    }
    
    if ([self.m_interstitialAd canYouShow:placement] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)showInterstitial:(NSString *)placement {
    if (self.m_interstitialAd == nil) {
        return NO;
    }
    
    if ([[iOSNetwork sharedInstance] isNetworkAvailable] == NO) {
        return NO;
    }
    
    if ([self.m_interstitialAd show:placement] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)isShowingInterstitial {
    if (self.m_interstitialAd == nil) {
        return NO;
    }
    
    if ([self.m_interstitialAd isShowing] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)hasRewarded {
    if (self.m_rewardedAd == nil) {
        return NO;
    }
    
    return YES;
}

- (BOOL)canOfferRewarded:(NSString *)placement {
    if (self.m_rewardedAd == nil) {
        return NO;
    }
    
    if ([[iOSNetwork sharedInstance] isNetworkAvailable] == NO) {
        return NO;
    }
    
    if ([self.m_rewardedAd canOffer:placement] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)canYouShowRewarded:(NSString *)placement {
    if (self.m_rewardedAd == nil) {
        return NO;
    }
    
    if ([[iOSNetwork sharedInstance] isNetworkAvailable] == NO) {
        return NO;
    }
    
    if ([self.m_rewardedAd canYouShow:placement] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)showRewarded:(NSString *)placement {
    if (self.m_rewardedAd == nil) {
        return NO;
    }
    
    if ([[iOSNetwork sharedInstance] isNetworkAvailable] == NO) {
        return NO;
    }
    
    if ([self.m_rewardedAd show:placement] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)isShowingRewarded {
    if (self.m_rewardedAd == nil) {
        return NO;
    }
    
    if ([self.m_rewardedAd isShowing] == NO) {
        return NO;
    }
    
    return YES;
}

@end

