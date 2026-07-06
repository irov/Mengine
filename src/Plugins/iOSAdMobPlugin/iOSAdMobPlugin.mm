#import "iOSAdMobPlugin.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleSemaphoreService.h"
#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSAppTrackingTransparencyParam.h"
#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSNetwork.h"
#import "Environment/iOS/iOSLog.h"

#import "Plugins/iOSAdvertisementPlugin/iOSAdvertisementInterface.h"

#include "Config/Version.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_DEBUG)
#   import <AdSupport/AdSupport.h>
#endif

@implementation iOSAdMobPlugin

- (instancetype)init {
    self = [super init];

    if (self) {
        self.m_bannerAd = nil;
        self.m_interstitialAd = nil;
        self.m_rewardedAd = nil;
        self.m_initialized = NO;
    }

    return self;
}

- (id<iOSAdvertisementCallbackInterface>)getAdvertisementBannerCallback {
    id<iOSAdvertisementInterface> advertisement = [iOSDetail getPluginDelegateOfProtocol:@protocol(iOSAdvertisementInterface)];

    id<iOSAdvertisementCallbackInterface> callback = [advertisement getBannerCallback];

    return callback;
}

- (id<iOSAdvertisementCallbackInterface>)getAdvertisementInterstitialCallback {
    id<iOSAdvertisementInterface> advertisement = [iOSDetail getPluginDelegateOfProtocol:@protocol(iOSAdvertisementInterface)];

    id<iOSAdvertisementCallbackInterface> callback = [advertisement getInterstitialCallback];

    return callback;
}

- (id<iOSAdvertisementCallbackInterface>)getAdvertisementRewardedCallback {
    id<iOSAdvertisementInterface> advertisement = [iOSDetail getPluginDelegateOfProtocol:@protocol(iOSAdvertisementInterface)];

    id<iOSAdvertisementCallbackInterface> callback = [advertisement getRewardedCallback];

    return callback;
}

- (iOSAdMobBannerDelegate *)getBanner {
    return self.m_bannerAd;
}

- (iOSAdMobInterstitialDelegate *)getInterstitial {
    return self.m_interstitialAd;
}

- (iOSAdMobRewardedDelegate *)getRewarded {
    return self.m_rewardedAd;
}

#pragma mark - iOSAdMobInterface

+ (instancetype)sharedInstance {
    static iOSAdMobPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[iOSAdMobPlugin class]];
    });
    return sharedInstance;
}

#pragma mark - iOSPluginAppTrackingTransparencyDelegateInterface

- (void)onAppTrackingTransparency:(iOSAppTrackingTransparencyParam *)param {
    if (self.m_initialized == YES) {
        return;
    }

    IOS_LOGGER_MESSAGE(@"[AdMob] ATT completed, initializing AdMob");

    [self initializeAdMob];
}

#pragma mark - iOSPluginInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if ([AppleBundle hasPluginConfig:@PLUGIN_BUNDLE_NAME] == NO) {
        IOS_LOGGER_ERROR(@"[ERROR] AdMob plugin not found bundle config [%@]", @PLUGIN_BUNDLE_NAME);

        return NO;
    }

    return YES;
}

- (void)initializeAdMob {
    if (self.m_initialized == YES) {
        return;
    }

    self.m_initialized = YES;

    IOS_LOGGER_MESSAGE(@"[AdMob] initializing after ATT completion");

    GADVersionNumber version = [[GADMobileAds sharedInstance] versionNumber];
    NSString * versionString = [NSString stringWithFormat:@"%lu.%lu.%lu", version.majorVersion, version.minorVersion, version.patchVersion];

    IOS_LOGGER_MESSAGE(@"AdMob: %@", versionString);

#if defined(MENGINE_DEBUG)
    BOOL MengineiOSAdMobPlugin_RequestConfigurationTestDeviceIdsEnabled = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"RequestConfigurationTestDeviceIdsEnabled" withDefault:NO];

    if (MengineiOSAdMobPlugin_RequestConfigurationTestDeviceIdsEnabled == YES) {
        GADRequestConfiguration * requestConfiguration = [GADMobileAds sharedInstance].requestConfiguration;

        if ([AppleDetail hasOption:@"admob.test_device_advertising"] == YES) {
            NSString * testDeviceId = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
            requestConfiguration.testDeviceIdentifiers = @[testDeviceId];

            IOS_LOGGER_MESSAGE(@"[AdMob] test device id: %@", testDeviceId);
        }
    }
#endif

    id<iOSAdvertisementInterface> advertisement = [iOSDetail getPluginDelegateOfProtocol:@protocol(iOSAdvertisementInterface)];

    [advertisement setProvider:self];

    NSString * MengineiOSAdMobPlugin_BannerAdUnitId = nil;
    NSString * MengineiOSAdMobPlugin_InterstitialAdUnitId = nil;
    NSString * MengineiOSAdMobPlugin_RewardedAdUnitId = nil;

#if defined(MENGINE_PLUGIN_IOS_ADMOB_BANNER)
    MengineiOSAdMobPlugin_BannerAdUnitId = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"BannerAdUnitId" withDefault:nil];
#endif

#if defined(MENGINE_PLUGIN_IOS_ADMOB_INTERSTITIAL)
    MengineiOSAdMobPlugin_InterstitialAdUnitId = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"InterstitialAdUnitId" withDefault:nil];
#endif

#if defined(MENGINE_PLUGIN_IOS_ADMOB_REWARDED)
    MengineiOSAdMobPlugin_RewardedAdUnitId = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"RewardedAdUnitId" withDefault:nil];
#endif

    [GADMobileAds sharedInstance].audioVideoManager.audioSessionIsApplicationManaged = YES;

    __weak iOSAdMobPlugin * weakSelf = self;

    [[GADMobileAds sharedInstance] startWithCompletionHandler:^(GADInitializationStatus * _Nonnull status) {
        iOSAdMobPlugin * strongSelf = weakSelf;

        if (strongSelf == nil) {
            return;
        }

        [AppleDetail addMainQueueOperation:^{
            IOS_LOGGER_MESSAGE(@"[AdMob] plugin initialize complete");
#if defined(MENGINE_PLUGIN_IOS_ADMOB_BANNER)
            if (MengineiOSAdMobPlugin_BannerAdUnitId != nil) {
                NSString * MengineiOSAdMobPlugin_BannerPlacement = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"BannerPlacement" withDefault:@"banner"];

                BOOL MengineiOSAdMobPlugin_BannerAdaptive = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"BannerAdaptive" withDefault:YES];

                iOSAdMobBannerDelegate * bannerAd = [[iOSAdMobBannerDelegate alloc] initWithAdUnitIdentifier:MengineiOSAdMobPlugin_BannerAdUnitId advertisement:advertisement placement:MengineiOSAdMobPlugin_BannerPlacement adaptive:MengineiOSAdMobPlugin_BannerAdaptive];

                strongSelf.m_bannerAd = bannerAd;
            }
#endif

#if defined(MENGINE_PLUGIN_IOS_ADMOB_INTERSTITIAL)
            if (MengineiOSAdMobPlugin_InterstitialAdUnitId != nil) {
                iOSAdMobInterstitialDelegate * interstitialAd = [[iOSAdMobInterstitialDelegate alloc] initWithAdUnitIdentifier:MengineiOSAdMobPlugin_InterstitialAdUnitId advertisement:advertisement];

                strongSelf.m_interstitialAd = interstitialAd;
            }
#endif

#if defined(MENGINE_PLUGIN_IOS_ADMOB_REWARDED)
            if (MengineiOSAdMobPlugin_RewardedAdUnitId != nil) {
                iOSAdMobRewardedDelegate * rewardedAd = [[iOSAdMobRewardedDelegate alloc] initWithAdUnitIdentifier:MengineiOSAdMobPlugin_RewardedAdUnitId advertisement:advertisement];

                strongSelf.m_rewardedAd = rewardedAd;
            }
#endif

            [advertisement readyAdProvider];
        }];
    }];
}

#pragma mark - iOSAdvertisementProviderInterface

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

    CGFloat withPx = [self.m_bannerAd getWidthPx];
    CGFloat heightPx = [self.m_bannerAd getHeightPx];

    *width = withPx;
    *height = heightPx;

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
