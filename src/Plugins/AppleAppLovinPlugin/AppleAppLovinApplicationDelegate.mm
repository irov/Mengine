#import "AppleAppLovinApplicationDelegate.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleSemaphoreService.h"
#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSNetwork.h"
#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSTransparencyConsentParam.h"

#import "Plugins/AppleAdvertisementPlugin/AppleAdvertisementInterface.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_META)
#   import <FBAudienceNetwork/FBAdSettings.h>
#endif

@implementation AppleAppLovinApplicationDelegate

- (id<AppleAdvertisementCallbackInterface>)getAdvertisementResponse {
    id<AppleAdvertisementInterface> advertisement = [iOSDetail getPluginDelegateOfProtocol:@protocol(AppleAdvertisementInterface)];
    
    id<AppleAdvertisementCallbackInterface> response = [advertisement getAdvertisementCallback];
    
    return response;
}

- (AppleAppLovinBannerDelegate *)getBanner {
    return self.m_bannerAd;
}

- (AppleAppLovinInterstitialDelegate *)getInterstitial {
    return self.m_interstitialAd;
}

- (AppleAppLovinRewardedDelegate *)getRewarded {
    return self.m_rewardedAd;
}

#pragma mark - AppleAppLovinInterface

+ (instancetype)sharedInstance {
    static AppleAppLovinApplicationDelegate * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleAppLovinApplicationDelegate class]];
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

- (BOOL)loadAndShowCMPFlow:(id<AppleAppLovinConsentFlowProviderInterface>)provider {
    ALCMPService * cmpService = [ALSdk shared].cmpService;

    [cmpService showCMPForExistingUserWithCompletion:^(ALCMPError * _Nullable error) {
        if (error != nil) {
            IOS_LOGGER_ERROR( @"AppLovin CMP show failed: %@ [%ld] message: %@"
                , error.message
                , error.code
                , error.cmpMessage
            );
            
            [AppleDetail dispatchMainQueue:^{
                [provider onAppleAppLovinConsentFlowShowFailed];
            }];
            
            return;
        }
        
        IOS_LOGGER_MESSAGE( @"AppLovin CMP show successful" );
        
        [AppleDetail dispatchMainQueue:^{
            [provider onAppleAppLovinConsentFlowShowSuccess];
        }];
    }];
}

- (void)showMediationDebugger {
    [[ALSdk shared] showMediationDebugger];
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if ([AppleBundle hasPluginConfig:@PLUGIN_BUNDLE_NAME] == NO) {
        IOS_LOGGER_MESSAGE(@"🔴 [ERROR] AppLovin plugin not found bundle config [%@]", @PLUGIN_BUNDLE_NAME);
        
        return NO;
    }
    
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_META)
    [FBAdSettings setDataProcessingOptions: @[]];
#endif
    
    IOS_LOGGER_MESSAGE(@"AppLovin: %@", ALSdk.version);
    
    NSString * MengineAppleAppLovinPlugin_CCPA = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"CCPA" withDefault:@"UNKNOWN"];
    
    if ([MengineAppleAppLovinPlugin_CCPA caseInsensitiveCompare:@"YES"] == NSOrderedSame) {
        [ALPrivacySettings setDoNotSell: YES];
    } else if ([MengineAppleAppLovinPlugin_CCPA caseInsensitiveCompare:@"NO"] == NSOrderedSame) {
        [ALPrivacySettings setDoNotSell: NO];
    } else if ([MengineAppleAppLovinPlugin_CCPA caseInsensitiveCompare:@"UNKNOWN"] == NSOrderedSame) {
        //Nothing
    } else {
        IOS_LOGGER_MESSAGE(@"🔴 [ERROR] AppLovin plugin invalid config [%@.CCPA] value %@ [YES|NO|UNKNOWN]", @PLUGIN_BUNDLE_NAME, MengineAppleAppLovinPlugin_CCPA);
        
        return NO;
    }
    
    NSString * MengineAppleAppLovinPlugin_SdkKey = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"SdkKey" withDefault:nil];
    
    if (MengineAppleAppLovinPlugin_SdkKey == nil) {
        IOS_LOGGER_MESSAGE(@"🔴 [ERROR] AppLovin plugin missed required config [%@.SdkKey]", @PLUGIN_BUNDLE_NAME);
        
        return NO;
    }
    
    ALSdkInitializationConfiguration * configuration = [ALSdkInitializationConfiguration configurationWithSdkKey:MengineAppleAppLovinPlugin_SdkKey builderBlock:^(ALSdkInitializationConfigurationBuilder * _Nonnull builder) {
        
        builder.mediationProvider = ALMediationProviderMAX;
    }];
    
    ALSdkSettings * settings = [ALSdk shared].settings;
    
    BOOL MengineAppleAppLovinPlugin_Verbose = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"Verbose" withDefault:NO];
    
    if (MengineAppleAppLovinPlugin_Verbose == YES) {
        settings.verboseLoggingEnabled = YES;
    } else {
        settings.verboseLoggingEnabled = NO;
    }
    
    NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
    
    settings.userIdentifier = sessionId;

    BOOL MengineAppleAppLovinPlugin_TermsAndPrivacyPolicyFlow = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"TermsAndPrivacyPolicyFlow" withDefault:NO];

    if (MengineAppleAppLovinPlugin_TermsAndPrivacyPolicyFlow == YES) {
        NSString * MengineAppleAppLovinPlugin_PrivacyPolicyURL = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"PrivacyPolicyURL" withDefault:nil];
        NSString * MengineAppleAppLovinPlugin_TermsOfServiceURL = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"TermsOfServiceURL" withDefault:nil];
        
        if (MengineAppleAppLovinPlugin_PrivacyPolicyURL == nil) {
            IOS_LOGGER_MESSAGE(@"🔴 [ERROR] AppLovin plugin missed required config [%@.PrivacyPolicyURL]", @PLUGIN_BUNDLE_NAME);
            
            return NO;
        }
        
        if (MengineAppleAppLovinPlugin_TermsOfServiceURL == nil) {
            IOS_LOGGER_MESSAGE(@"🔴 [ERROR] AppLovin plugin missed required config [%@.TermsOfServiceURL]", @PLUGIN_BUNDLE_NAME);
            
            return NO;
        }
        
        settings.termsAndPrivacyPolicyFlowSettings.enabled = YES;
        settings.termsAndPrivacyPolicyFlowSettings.privacyPolicyURL = [NSURL URLWithString:MengineAppleAppLovinPlugin_PrivacyPolicyURL];
        settings.termsAndPrivacyPolicyFlowSettings.termsOfServiceURL = [NSURL URLWithString:MengineAppleAppLovinPlugin_TermsOfServiceURL];
    } else {
        settings.termsAndPrivacyPolicyFlowSettings.enabled = NO;
    }
    
    [[ALSdk shared] initializeWithConfiguration:configuration completionHandler:^(ALSdkConfiguration *configuration) {
        ALConsentFlowUserGeography consentFlowUserGeography = configuration.consentFlowUserGeography;
        NSString * countryCode = configuration.countryCode;
        ALAppTrackingTransparencyStatus appTrackingTransparencyStatus = configuration.appTrackingTransparencyStatus;
        BOOL testModeEnabled = configuration.testModeEnabled;
        
        IOS_LOGGER_MESSAGE(@"[AppLovin] plugin initialize complete");
        IOS_LOGGER_MESSAGE(@"[AppLovin] consent flow user geography: %ld", consentFlowUserGeography);
        IOS_LOGGER_MESSAGE(@"[AppLovin] country code: %@", countryCode);
        IOS_LOGGER_MESSAGE(@"[AppLovin] app tracking transparency status: %ld", appTrackingTransparencyStatus);
        IOS_LOGGER_MESSAGE(@"[AppLovin] test mode enabled: %d", testModeEnabled);
        
        iOSTransparencyConsentParam * consent = [[iOSTransparencyConsentParam alloc] initFromUserDefaults];
        
        [iOSDetail transparencyConsent:consent];
        
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_META)
        BOOL ATTAllowed = [iOSDetail isAppTrackingTransparencyAllowed];
        [FBAdSettings setAdvertiserTrackingEnabled:ATTAllowed];
        
        IOS_LOGGER_MESSAGE(@"FBAdSettings setAdvertiserTrackingEnabled:%d", ATTAllowed);
#endif
        
        NSString * MengineAppleAppLovinPlugin_BannerAdUnitId = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"BannerAdUnitId" withDefault:nil];
        
        if (MengineAppleAppLovinPlugin_BannerAdUnitId != nil) {
            NSString * MengineAppleAppLovinPlugin_BannerPlacement = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"BannerPlacement" withDefault:@"banner"];
            
            BOOL MengineAppleAppLovinPlugin_BannerAdaptive = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"BannerAdaptive" withDefault:YES];
            
            AppleAppLovinBannerDelegate * bannerAd = [[AppleAppLovinBannerDelegate alloc] initWithAdUnitIdentifier:MengineAppleAppLovinPlugin_BannerAdUnitId placement:MengineAppleAppLovinPlugin_BannerPlacement adaptive:MengineAppleAppLovinPlugin_BannerAdaptive];
            
            self.m_bannerAd = bannerAd;
        }
        
        NSString * MengineAppleAppLovinPlugin_InterstitialAdUnitId = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"InterstitialAdUnitId" withDefault:nil];
        
        if (MengineAppleAppLovinPlugin_InterstitialAdUnitId != nil) {
            AppleAppLovinInterstitialDelegate * interstitialAd = [[AppleAppLovinInterstitialDelegate alloc] initWithAdUnitIdentifier:MengineAppleAppLovinPlugin_InterstitialAdUnitId];
            
            self.m_interstitialAd = interstitialAd;
        }
        
        NSString * MengineAppleAppLovinPlugin_RewardedAdUnitId = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"RewardedAdUnitId" withDefault:nil];
        
        if (MengineAppleAppLovinPlugin_RewardedAdUnitId != nil) {
            AppleAppLovinRewardedDelegate * rewardedAd = [[AppleAppLovinRewardedDelegate alloc] initWithAdUnitIdentifier:MengineAppleAppLovinPlugin_RewardedAdUnitId];
            
            self.m_rewardedAd = rewardedAd;
        }
        
        id<AppleAdvertisementInterface> advertisement = [iOSDetail getPluginDelegateOfProtocol:@protocol(AppleAdvertisementInterface)];
        
        [advertisement setAdvertisementProvider:self];
        
        if ([AppleDetail hasOption:@"applovin.show_mediation_debugger"] == YES) {
            [[ALSdk shared] showMediationDebugger];
        }
        
        [AppleSemaphoreService.sharedInstance activateSemaphore:@"AppLovinSdkInitialized"];
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

@end
