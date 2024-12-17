#import "AppleAppLovinApplicationDelegate.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleLog.h"
#import "Environment/Apple/AppleSemaphoreService.h"

#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSTransparencyConsentParam.h"

#include "Kernel/ThreadHelper.h"
#include "Kernel/ConstStringHelper.h"

#include "Configuration/Configurations.h"

#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_META)
#   import <FBAudienceNetwork/FBAdSettings.h>
#endif

#define PLUGIN_BUNDLE_NAME "MengineAppleAppLovinPlugin"

@implementation AppleAppLovinApplicationDelegate

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if( [AppleBundle hasPluginConfig:@PLUGIN_BUNDLE_NAME] == NO ) {
        [AppleLog withFormat:@"🔴 [ERROR] AppLovin plugin not found bundle config [%@]", @PLUGIN_BUNDLE_NAME];
        
        return NO;
    }
    
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_META)
    [FBAdSettings setDataProcessingOptions: @[]];
#endif
    
    [AppleLog withFormat:@"AppLovin: %@", ALSdk.version];
    
    NSString * MengineAppleAppLovinPlugin_CCPA = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"CCPA" withDefault:@"UNKNOWN"];
    
    if( [MengineAppleAppLovinPlugin_CCPA caseInsensitiveCompare:@"YES"] == NSOrderedSame ) {
        [ALPrivacySettings setDoNotSell: YES];
    } else if( [MengineAppleAppLovinPlugin_CCPA caseInsensitiveCompare:@"NO"] == NSOrderedSame ) {
        [ALPrivacySettings setDoNotSell: NO];
    } else if( [MengineAppleAppLovinPlugin_CCPA caseInsensitiveCompare:@"UNKNOWN"] == NSOrderedSame ) {
        //Nothing
    } else {
        [AppleLog withFormat:@"🔴 [ERROR] AppLovin plugin invalid config [%@.CCPA] value %@ [YES|NO|UNKNOWN]", @PLUGIN_BUNDLE_NAME, MengineAppleAppLovinPlugin_CCPA];
        
        return NO;
    }
    
    NSString * MengineAppleAppLovinPlugin_SdkKey = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"SdkKey" withDefault:nil];
    
    if( MengineAppleAppLovinPlugin_SdkKey == nil ) {
        [AppleLog withFormat:@"🔴 [ERROR] AppLovin plugin missed required config [%@.SdkKey]", @PLUGIN_BUNDLE_NAME];
        
        return NO;
    }
    
    ALSdkInitializationConfiguration * configuration = [ALSdkInitializationConfiguration configurationWithSdkKey:MengineAppleAppLovinPlugin_SdkKey builderBlock:^(ALSdkInitializationConfigurationBuilder * _Nonnull builder) {
        
        builder.pluginVersion = @"Mengine-AppLovin-Apple-1.0.0";
        builder.mediationProvider = ALMediationProviderMAX;
    }];
    
    ALSdkSettings * settings = [ALSdk shared].settings;
    
    BOOL MengineAppleAppLovinPlugin_Verbose = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"Verbose" withDefault:NO];
    
    if( MengineAppleAppLovinPlugin_Verbose == YES ) {
        settings.verboseLoggingEnabled = YES;
    } else {
        settings.verboseLoggingEnabled = NO;
    }
    
    NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
    
    settings.userIdentifier = sessionId;

    BOOL MengineAppleAppLovinPlugin_TermsAndPrivacyPolicyFlow = [AppleBundle getPluginConfigBoolean:@PLUGIN_BUNDLE_NAME withKey:@"TermsAndPrivacyPolicyFlow" withDefault:NO];

    if( MengineAppleAppLovinPlugin_TermsAndPrivacyPolicyFlow == YES ) {
        NSString * MengineAppleAppLovinPlugin_PrivacyPolicyURL = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"PrivacyPolicyURL" withDefault:nil];
        NSString * MengineAppleAppLovinPlugin_TermsOfServiceURL = [AppleBundle getPluginConfigString:@PLUGIN_BUNDLE_NAME withKey:@"TermsOfServiceURL" withDefault:nil];
        
        if( MengineAppleAppLovinPlugin_PrivacyPolicyURL == nil ) {
            [AppleLog withFormat:@"🔴 [ERROR] AppLovin plugin missed required config [%@.PrivacyPolicyURL]", @PLUGIN_BUNDLE_NAME];
            
            return NO;
        }
        
        if( MengineAppleAppLovinPlugin_TermsOfServiceURL == nil ) {
            [AppleLog withFormat:@"🔴 [ERROR] AppLovin plugin missed required config [%@.TermsOfServiceURL]", @PLUGIN_BUNDLE_NAME];
            
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
        
        [AppleLog withFormat:@"[AppLovin] plugin initialize complete"];
        [AppleLog withFormat:@"[AppLovin] consent flow user geography: %ld", consentFlowUserGeography];
        [AppleLog withFormat:@"[AppLovin] country code: %@", countryCode];
        [AppleLog withFormat:@"[AppLovin] app tracking transparency status: %ld", appTrackingTransparencyStatus];
        [AppleLog withFormat:@"[AppLovin] test mode enabled: %d", testModeEnabled];
        
        iOSTransparencyConsentParam * consent = [[iOSTransparencyConsentParam alloc] initFromUserDefaults];
        
        [iOSDetail transparencyConsent:consent];
        
#if defined(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_META)
        BOOL ATTAllowed = [iOSDetail isAppTrackingTransparencyAllowed];
        [FBAdSettings setAdvertiserTrackingEnabled:ATTAllowed];
        
        [AppleLog withFormat:@"FBAdSettings setAdvertiserTrackingEnabled:%d", ATTAllowed];
#endif
        
        if ([[[NSProcessInfo processInfo] arguments] containsObject:@"-applovin.show_mediation_debugger"] == YES) {
            [[ALSdk shared] showMediationDebugger];
        }
        
        [AppleSemaphoreService.sharedInstance activateSemaphore:@"AppLovinSdkInitialized"];
    }];
    
    return YES;
}

@end
