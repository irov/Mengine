#import "iOSAppLovinPlugin.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleSemaphoreService.h"
#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSNetwork.h"
#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSTransparencyConsentParam.h"

#import "Plugins/iOSAdvertisementPlugin/iOSAdvertisementInterface.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "iOSAppLovinScriptEmbedding.h"
#endif

#include "Config/Version.h"

#if defined(MENGINE_DEBUG)
#   import <AdSupport/AdSupport.h>
#endif

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_MEDIATION_META)
#   import <FBAudienceNetwork/FBAdSettings.h>
#endif

@implementation iOSAppLovinPlugin

- (instancetype)init {
    self = [super init];

    if (self) {
        self.m_bannerAd = nil;
        self.m_interstitialAd = nil;
        self.m_rewardedAd = nil;

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_MEDIATION_AMAZON)
        self.m_amazonService = nil;
#endif
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

- (iOSAppLovinBannerDelegate *)getBanner {
    return self.m_bannerAd;
}

- (iOSAppLovinInterstitialDelegate *)getInterstitial {
    return self.m_interstitialAd;
}

- (iOSAppLovinRewardedDelegate *)getRewarded {
    return self.m_rewardedAd;
}

#pragma mark - iOSAppLovinInterface

+ (instancetype)sharedInstance {
    static iOSAppLovinPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[iOSAppLovinPlugin class]];
    });
    return sharedInstance;
}

- (BOOL)hasSupportedCMP {
    ALCMPService * cmpService = [ALSdk shared].cmpService;

    if ([cmpService hasSupportedCMP] == NO) {
        return NO;
    }

    return YES;
}

- (BOOL)isConsentFlowUserGeographyGDPR {
    ALSdkConfiguration * configuration = [ALSdk shared].configuration;

    if (configuration.consentFlowUserGeography != ALConsentFlowUserGeographyGDPR) {
        return NO;
    }

    return YES;
}

- (BOOL)loadAndShowCMPFlow:(id<iOSAppLovinConsentFlowProviderInterface>)provider {
    ALCMPService * cmpService = [ALSdk shared].cmpService;

    [cmpService showCMPForExistingUserWithCompletion:^(ALCMPError * _Nullable error) {
        if (error != nil) {
            IOS_LOGGER_ERROR( @"AppLovin CMP show failed: %@ [%ld] message: %@"
                , error.message
                , error.code
                , error.cmpMessage
            );

            [AppleDetail addMainQueueOperation:^{
                [provider oniOSAppLovinConsentFlowShowFailed];
            }];

            return;
        }

        IOS_LOGGER_MESSAGE( @"AppLovin CMP show success" );

        [AppleDetail addMainQueueOperation:^{
            [provider oniOSAppLovinConsentFlowShowSuccess];
        }];
    }];
}

- (void)showMediationDebugger {
    [[ALSdk shared] showMediationDebugger];
}

#pragma mark - iOSPluginInterface

- (void)onRunBegin {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::iOSAppLovinScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE );
#endif
}

- (void)onStopEnd {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::iOSAppLovinScriptEmbedding>();
#endif
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if ([AppleBundle hasPluginConfig:@PLUGIN_BUNDLE_NAME] == NO) {
        IOS_LOGGER_ERROR(@"[ERROR] AppLovin plugin not found bundle config [%@]", @PLUGIN_BUNDLE_NAME);

        return NO;
    }

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_MEDIATION_META)
    [FBAdSettings setDataProcessingOptions: @[]];
#endif

    IOS_LOGGER_MESSAGE(@"AppLovin: %@ [%lu]", ALSdk.version, ALSdk.versionCode);

    ALSdkSettings * settings = [ALSdk shared].settings;

    BOOL MengineiOSAppLovinPlugin_VerboseLoggingEnabled = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"VerboseLoggingEnabled" withDefault:NO];

    if (MengineiOSAppLovinPlugin_VerboseLoggingEnabled == YES) {
        settings.verboseLoggingEnabled = YES;
    } else {
        settings.verboseLoggingEnabled = NO;
    }

    BOOL MengineiOSAppLovinPlugin_CreativeDebuggerEnabled = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"CreativeDebuggerEnabled" withDefault:NO];

    if (MengineiOSAppLovinPlugin_CreativeDebuggerEnabled == YES) {
        settings.creativeDebuggerEnabled = YES;
    } else {
        settings.creativeDebuggerEnabled = NO;
    }

    NSString * userId = [iOSApplication.sharedInstance getUserId];

    settings.userIdentifier = userId;

    BOOL MengineiOSAppLovinPlugin_TermsAndPrivacyPolicyFlow = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"TermsAndPrivacyPolicyFlow" withDefault:NO];

    if (MengineiOSAppLovinPlugin_TermsAndPrivacyPolicyFlow == YES) {
        NSString * MengineiOSAppLovinPlugin_PrivacyPolicyURL = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"PrivacyPolicyURL" withDefault:nil];
        NSString * MengineiOSAppLovinPlugin_TermsOfServiceURL = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"TermsOfServiceURL" withDefault:nil];

        if (MengineiOSAppLovinPlugin_PrivacyPolicyURL == nil) {
            IOS_LOGGER_ERROR(@"[ERROR] AppLovin plugin missed required config [%@.PrivacyPolicyURL]", @PLUGIN_BUNDLE_NAME);

            return NO;
        }

        if (MengineiOSAppLovinPlugin_TermsOfServiceURL == nil) {
            IOS_LOGGER_ERROR(@"[ERROR] AppLovin plugin missed required config [%@.TermsOfServiceURL]", @PLUGIN_BUNDLE_NAME);

            return NO;
        }

        settings.termsAndPrivacyPolicyFlowSettings.enabled = YES;
        settings.termsAndPrivacyPolicyFlowSettings.privacyPolicyURL = [NSURL URLWithString:MengineiOSAppLovinPlugin_PrivacyPolicyURL];
        settings.termsAndPrivacyPolicyFlowSettings.termsOfServiceURL = [NSURL URLWithString:MengineiOSAppLovinPlugin_TermsOfServiceURL];
        settings.termsAndPrivacyPolicyFlowSettings.showTermsAndPrivacyPolicyAlertInGDPR = NO;
    } else {
        settings.termsAndPrivacyPolicyFlowSettings.enabled = NO;
    }

    NSString * MengineiOSAppLovinPlugin_PrivacyDoNoSell = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"PrivacyDoNoSell" withDefault:@"UNKNOWN"];

    if ([MengineiOSAppLovinPlugin_PrivacyDoNoSell caseInsensitiveCompare:@"YES"] == NSOrderedSame) {
        [ALPrivacySettings setDoNotSell: YES];
    } else if ([MengineiOSAppLovinPlugin_PrivacyDoNoSell caseInsensitiveCompare:@"NO"] == NSOrderedSame) {
        [ALPrivacySettings setDoNotSell: NO];
    } else if ([MengineiOSAppLovinPlugin_PrivacyDoNoSell caseInsensitiveCompare:@"UNKNOWN"] == NSOrderedSame) {
        //Nothing
    } else {
        IOS_LOGGER_ERROR(@"[ERROR] AppLovin plugin invalid config [%@.PrivacyDoNoSell] value %@ [YES|NO|UNKNOWN]", @PLUGIN_BUNDLE_NAME, MengineiOSAppLovinPlugin_PrivacyDoNoSell);

        return NO;
    }

    id<iOSAdvertisementInterface> advertisement = [iOSDetail getPluginDelegateOfProtocol:@protocol(iOSAdvertisementInterface)];

    [advertisement setProvider:self];

    NSMutableArray * adUnitIdentifiers = [NSMutableArray array];

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_BANNER)
        NSString * MengineiOSAppLovinPlugin_BannerAdUnitId = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"BannerAdUnitId" withDefault:nil];

        if (MengineiOSAppLovinPlugin_BannerAdUnitId != nil) {
            [adUnitIdentifiers addObject:MengineiOSAppLovinPlugin_BannerAdUnitId];
        }
#endif

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_INTERSTITIAL)
        NSString * MengineiOSAppLovinPlugin_InterstitialAdUnitId = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"InterstitialAdUnitId" withDefault:nil];

        if (MengineiOSAppLovinPlugin_InterstitialAdUnitId != nil) {
            [adUnitIdentifiers addObject:MengineiOSAppLovinPlugin_InterstitialAdUnitId];
        }
#endif

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_REWARDED)
        NSString * MengineiOSAppLovinPlugin_RewardedAdUnitId = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"RewardedAdUnitId" withDefault:nil];

        if (MengineiOSAppLovinPlugin_RewardedAdUnitId != nil) {
            [adUnitIdentifiers addObject:MengineiOSAppLovinPlugin_RewardedAdUnitId];
        }
#endif

    NSString * MengineiOSAppLovinPlugin_SdkKey = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"SdkKey" withDefault:nil];

    if (MengineiOSAppLovinPlugin_SdkKey == nil) {
        IOS_LOGGER_ERROR(@"[ERROR] AppLovin plugin missed required config [%@.SdkKey]", @PLUGIN_BUNDLE_NAME);

        return NO;
    }

    ALSdkInitializationConfiguration * initializationConfiguration = [ALSdkInitializationConfiguration configurationWithSdkKey:MengineiOSAppLovinPlugin_SdkKey builderBlock:^(ALSdkInitializationConfigurationBuilder * _Nonnull builder) {
        builder.pluginVersion = [@"Mengine-v" stringByAppendingString:@MENGINE_ENGINE_VERSION_STRING];
        builder.mediationProvider = ALMediationProviderMAX;
        builder.adUnitIdentifiers = adUnitIdentifiers;
#if defined(MENGINE_DEBUG)
        if ([AppleDetail hasOption:@"applovin.test_device_advertising"] == YES) {
            builder.testDeviceAdvertisingIdentifiers = @[[ASIdentifierManager sharedManager].advertisingIdentifier.UUIDString];
        }
#endif
    }];

    [[ALSdk shared] initializeWithConfiguration:initializationConfiguration completionHandler:^(ALSdkConfiguration *configuration) {
        ALConsentFlowUserGeography consentFlowUserGeography = configuration.consentFlowUserGeography;
        NSString * countryCode = configuration.countryCode;
        ALAppTrackingTransparencyStatus appTrackingTransparencyStatus = configuration.appTrackingTransparencyStatus;
        BOOL testModeEnabled = configuration.testModeEnabled;

        IOS_LOGGER_MESSAGE(@"[AppLovin] plugin initialize complete");
        IOS_LOGGER_MESSAGE(@"[AppLovin] consent flow user geography: %ld", consentFlowUserGeography);
        IOS_LOGGER_MESSAGE(@"[AppLovin] country code: %@", countryCode);
        IOS_LOGGER_MESSAGE(@"[AppLovin] app tracking transparency status: %ld", appTrackingTransparencyStatus);
        IOS_LOGGER_MESSAGE(@"[AppLovin] test mode enabled: %d", testModeEnabled);

        switch (consentFlowUserGeography) {
            case ALConsentFlowUserGeographyGDPR:
                [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyGDPR];
                break;
            case ALConsentFlowUserGeographyOther:
                [iOSTransparencyConsentParam setConsentFlowUserGeography:iOSConsentFlowUserGeographyOther];
                break;
            default:
                break;
        }

        iOSTransparencyConsentParam * consent = [[iOSTransparencyConsentParam alloc] initFromUserDefaults];

        [iOSDetail transparencyConsent:consent];

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_MEDIATION_META)
        BOOL ATTAllowed = [iOSDetail isAppTrackingTransparencyAllowed];
        [FBAdSettings setAdvertiserTrackingEnabled:ATTAllowed];

        IOS_LOGGER_MESSAGE(@"FBAdSettings setAdvertiserTrackingEnabled:%d", ATTAllowed);
#endif

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_BANNER)
        if (MengineiOSAppLovinPlugin_BannerAdUnitId != nil) {
            NSString * MengineiOSAppLovinPlugin_BannerPlacement = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"BannerPlacement" withDefault:@"banner"];

            BOOL MengineiOSAppLovinPlugin_BannerAdaptive = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"BannerAdaptive" withDefault:YES];

            iOSAppLovinBannerDelegate * bannerAd = [[iOSAppLovinBannerDelegate alloc] initWithAdUnitIdentifier:MengineiOSAppLovinPlugin_BannerAdUnitId advertisement:advertisement placement:MengineiOSAppLovinPlugin_BannerPlacement adaptive:MengineiOSAppLovinPlugin_BannerAdaptive];

            self.m_bannerAd = bannerAd;
        }
#endif

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_INTERSTITIAL)
        if (MengineiOSAppLovinPlugin_InterstitialAdUnitId != nil) {
            iOSAppLovinInterstitialDelegate * interstitialAd = [[iOSAppLovinInterstitialDelegate alloc] initWithAdUnitIdentifier:MengineiOSAppLovinPlugin_InterstitialAdUnitId advertisement:advertisement];

            self.m_interstitialAd = interstitialAd;
        }
#endif

#if defined(MENGINE_PLUGIN_IOS_APPLOVIN_REWARDED)
        if (MengineiOSAppLovinPlugin_RewardedAdUnitId != nil) {
            iOSAppLovinRewardedDelegate * rewardedAd = [[iOSAppLovinRewardedDelegate alloc] initWithAdUnitIdentifier:MengineiOSAppLovinPlugin_RewardedAdUnitId advertisement:advertisement];

            self.m_rewardedAd = rewardedAd;
        }
#endif

        [advertisement readyAdProvider];

        if ([AppleDetail hasOption:@"applovin.show_mediation_debugger"] == YES) {
            [[ALSdk shared] showMediationDebugger];
        }
    }];

    return YES;
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
