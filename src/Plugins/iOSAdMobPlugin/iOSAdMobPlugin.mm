#import "iOSAdMobPlugin.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleSemaphoreService.h"
#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSAppTrackingTransparencyParam.h"
#import "Environment/iOS/iOSTransparencyConsentParam.h"
#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSNetwork.h"
#import "Environment/iOS/iOSLog.h"

#import "Plugins/iOSAdvertisementPlugin/iOSAdvertisementInterface.h"
#import "Plugins/iOSFirebaseRemoteConfigPlugin/iOSFirebaseRemoteConfigInterface.h"

#include "Config/Version.h"

#import "Configuration/Configurations.h"

#if defined(MENGINE_DEBUG)
#   import <AdSupport/AdSupport.h>
#endif

@implementation iOSAdMobPlugin

- (instancetype)init {
    self = [super init];

    if (self) {
        self.m_bannerAd = nil;
        self.m_topperAd = nil;
        self.m_interstitialAd = nil;
        self.m_rewardedAd = nil;
        self.m_rewardedInterstitialAd = nil;
        self.m_initializationAttempted = NO;
        self.m_consentCompleted = NO;
        self.m_canRequestAds = NO;
        self.m_appTrackingCompleted = NO;
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
    self.m_appTrackingCompleted = YES;

    [self tryInitializeAdMob];
}

#pragma mark - iOSPluginTransparencyConsentDelegateInterface

- (void)onTransparencyConsent:(iOSTransparencyConsentParam *)consent {
    self.m_consentCompleted = YES;
    self.m_canRequestAds = [consent canRequestAds];

    [self tryInitializeAdMob];
}

- (void)tryInitializeAdMob {
    if (self.m_initializationAttempted == YES) {
        return;
    }

    if (self.m_consentCompleted == NO) {
        return;
    }

    if (self.m_appTrackingCompleted == NO) {
        return;
    }

    if (self.m_canRequestAds == NO) {
        IOS_LOGGER_MESSAGE(@"[AdMob] consent does not allow ad requests, skipping initialization");
        return;
    }

    IOS_LOGGER_MESSAGE(@"[AdMob] consent and ATT flow completed, initializing AdMob");

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
    if (self.m_initializationAttempted == YES) {
        return;
    }

    self.m_initializationAttempted = YES;

    id<iOSFirebaseRemoteConfigInterface> remoteConfig = [iOSDetail getPluginDelegateOfProtocol:@protocol(iOSFirebaseRemoteConfigInterface)];
    NSDictionary * adUnitConfig = [remoteConfig getRemoteConfigValue:@"admob_ad_units"];

    if (adUnitConfig == nil) {
        IOS_LOGGER_MESSAGE(@"[AdMob] no cached Remote Config admob_ad_units, skipping initialization for this session");

        return;
    }

    Class dictionaryClass = [NSDictionary class];
    BOOL validConfig = [adUnitConfig isKindOfClass:dictionaryClass];

    if (validConfig == NO) {
        IOS_LOGGER_ERROR(@"[AdMob] Remote Config admob_ad_units must be a JSON object");

        return;
    }

    NSArray<NSString *> * formats = @[@"banner", @"topper", @"interstitial", @"rewarded", @"rewarded_interstitial"];
    NSMutableDictionary<NSString *, NSString *> * adUnitIds = [NSMutableDictionary dictionary];

    Class stringClass = [NSString class];

    for (NSString * format in formats) {
        id value = [adUnitConfig objectForKey:format];

        if (value == nil) {
            continue;
        }

        BOOL validId = [value isKindOfClass:stringClass];

        if (validId == NO) {
            IOS_LOGGER_ERROR(@"[AdMob] Remote Config admob_ad_units.%@ must be a string", format);

            return;
        }

        NSString * adUnitId = [value copy];

        if (adUnitId.length == 0) {
            continue;
        }

        [adUnitIds setObject:adUnitId forKey:format];
    }

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

    NSString * bannerAdUnitId = nil;
    NSString * interstitialAdUnitId = nil;
    NSString * rewardedAdUnitId = nil;
    NSString * rewardedInterstitialAdUnitId = nil;

#if defined(MENGINE_PLUGIN_IOS_ADMOB_BANNER)
    bannerAdUnitId = [adUnitIds objectForKey:@"banner"];
    NSString * topperAdUnitId = [adUnitIds objectForKey:@"topper"];
#endif

#if defined(MENGINE_PLUGIN_IOS_ADMOB_INTERSTITIAL)
    interstitialAdUnitId = [adUnitIds objectForKey:@"interstitial"];
#endif

#if defined(MENGINE_PLUGIN_IOS_ADMOB_REWARDED)
    rewardedAdUnitId = [adUnitIds objectForKey:@"rewarded"];
#endif

#if defined(MENGINE_PLUGIN_IOS_ADMOB_REWARDED_INTERSTITIAL)
    rewardedInterstitialAdUnitId = [adUnitIds objectForKey:@"rewarded_interstitial"];
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
            BOOL bannerAdaptive = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"BannerAdaptive" withDefault:YES];

            if (bannerAdUnitId != nil) {
                NSString * bannerPlacement = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"BannerPlacement" withDefault:@"banner"];

                strongSelf.m_bannerAd = [[iOSAdMobBannerDelegate alloc] initWithAdUnitIdentifier:bannerAdUnitId advertisement:advertisement placement:bannerPlacement anchor:IOS_ADVERTISEMENT_BANNER_ANCHOR_BOTTOM adaptive:bannerAdaptive];
            }

            if (topperAdUnitId != nil) {
                strongSelf.m_topperAd = [[iOSAdMobBannerDelegate alloc] initWithAdUnitIdentifier:topperAdUnitId advertisement:advertisement placement:@"topper" anchor:IOS_ADVERTISEMENT_BANNER_ANCHOR_TOP adaptive:bannerAdaptive];
            }
#endif

#if defined(MENGINE_PLUGIN_IOS_ADMOB_INTERSTITIAL)
            if (interstitialAdUnitId != nil) {
                iOSAdMobInterstitialDelegate * interstitialAd = [[iOSAdMobInterstitialDelegate alloc] initWithAdUnitIdentifier:interstitialAdUnitId advertisement:advertisement];

                strongSelf.m_interstitialAd = interstitialAd;
            }
#endif

#if defined(MENGINE_PLUGIN_IOS_ADMOB_REWARDED)
            if (rewardedAdUnitId != nil) {
                iOSAdMobRewardedDelegate * rewardedAd = [[iOSAdMobRewardedDelegate alloc] initWithAdUnitIdentifier:rewardedAdUnitId advertisement:advertisement];

                strongSelf.m_rewardedAd = rewardedAd;
            }
#endif

#if defined(MENGINE_PLUGIN_IOS_ADMOB_REWARDED_INTERSTITIAL)
            if (rewardedInterstitialAdUnitId != nil) {
                strongSelf.m_rewardedInterstitialAd = [[iOSAdMobRewardedInterstitialDelegate alloc] initWithAdUnitIdentifier:rewardedInterstitialAdUnitId advertisement:advertisement];
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

- (BOOL)isBannerLoaded {
    if (self.m_bannerAd == nil) {
        return NO;
    }

    if (self.m_bannerAd.m_bannerLoaded == NO) {
        return NO;
    }

    return YES;
}

- (void)showBanner {
    if (self.m_bannerAd == nil) {
        return;
    }

    [self.m_bannerAd show];
}

- (void)hideBanner {
    if (self.m_bannerAd == nil) {
        return;
    }

    [self.m_bannerAd hide];
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

- (BOOL)hasTopper {
    if (self.m_topperAd == nil) {
        return NO;
    }

    return YES;
}

- (BOOL)isTopperLoaded {
    if (self.m_topperAd == nil) {
        return NO;
    }

    if (self.m_topperAd.m_bannerLoaded == NO) {
        return NO;
    }

    return YES;
}

- (void)showTopper {
    if (self.m_topperAd == nil) {
        return;
    }

    [self.m_topperAd show];
}

- (void)hideTopper {
    if (self.m_topperAd == nil) {
        return;
    }

    [self.m_topperAd hide];
}

- (BOOL)getTopperWidth:(uint32_t *)width height:(uint32_t *)height {
    if (self.m_topperAd == nil) {
        return NO;
    }

    CGFloat widthPx = [self.m_topperAd getWidthPx];
    CGFloat heightPx = [self.m_topperAd getHeightPx];

    *width = widthPx;
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
    if ([self isShowingInterstitial] == YES) {
        return NO;
    }

    if ([self isShowingRewarded] == YES) {
        return NO;
    }

    if ([self isShowingRewardedInterstitial] == YES) {
        return NO;
    }

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
    if ([self isShowingInterstitial] == YES) {
        return NO;
    }

    if ([self isShowingRewarded] == YES) {
        return NO;
    }

    if ([self isShowingRewardedInterstitial] == YES) {
        return NO;
    }

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
    if ([self isShowingInterstitial] == YES) {
        return NO;
    }

    if ([self isShowingRewarded] == YES) {
        return NO;
    }

    if ([self isShowingRewardedInterstitial] == YES) {
        return NO;
    }

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
    if ([self isShowingInterstitial] == YES) {
        return NO;
    }

    if ([self isShowingRewarded] == YES) {
        return NO;
    }

    if ([self isShowingRewardedInterstitial] == YES) {
        return NO;
    }

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

- (BOOL)hasRewardedInterstitial {
    return self.m_rewardedInterstitialAd != nil;
}

- (BOOL)canYouShowRewardedInterstitial:(NSString *)placement {
    if ([self isShowingInterstitial] == YES) {
        return NO;
    }

    if ([self isShowingRewarded] == YES) {
        return NO;
    }

    if ([self isShowingRewardedInterstitial] == YES) {
        return NO;
    }

    BOOL canShow = [self.m_rewardedInterstitialAd canYouShow:placement];

    return canShow;
}

- (BOOL)showRewardedInterstitial:(NSString *)placement {
    if ([self canYouShowRewardedInterstitial:placement] == NO) {
        return NO;
    }

    BOOL shown = [self.m_rewardedInterstitialAd show:placement];

    return shown;
}

- (BOOL)isShowingRewardedInterstitial {
    BOOL showing = [self.m_rewardedInterstitialAd isShowing];

    return showing;
}

@end
