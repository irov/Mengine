#import "AppleAppLovinApplicationDelegate.h"

#include "Environment/Apple/AppleBundle.h"
#include "Environment/Apple/AppleLog.h"

#include "Kernel/ThreadHelper.h"
#include "Kernel/SemaphoreHelper.h"
#include "Kernel/ConstStringHelper.h"

@implementation AppleAppLovinApplicationDelegate

static ALSdk * AppLovinSdk;

#define PLUGIN_BUNDLE_NAME @"MengineAppleAppLovinPlugin"

+ (ALSdk * _Nonnull) AppLovinSdk {    
    return AppLovinSdk;
}

+ (void) setAppLovinSdk:(ALSdk * _Nonnull) value {
    AppLovinSdk = value;
}

#pragma mark - UIPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if( Mengine::Helper::AppleHasBundlePluginConfig(PLUGIN_BUNDLE_NAME) == NO )
    {
        Mengine::Helper::AppleLog(@"🔴 [ERROR] AppLovin plugin not found bundle config [%@]", PLUGIN_BUNDLE_NAME);
        
        return NO;
    }
    
    NSString * MengineAppleAppLovinPlugin_IsAgeRestrictedUser = Mengine::Helper::AppleGetBundlePluginConfigString(PLUGIN_BUNDLE_NAME, @"IsAgeRestrictedUser", @"UNKNOWN");
    
    if( [MengineAppleAppLovinPlugin_IsAgeRestrictedUser caseInsensitiveCompare:@"YES"] == NSOrderedSame )
    {
        [ALPrivacySettings setIsAgeRestrictedUser: YES];
    }
    else if( [MengineAppleAppLovinPlugin_IsAgeRestrictedUser caseInsensitiveCompare:@"NO"] == NSOrderedSame )
    {
        [ALPrivacySettings setIsAgeRestrictedUser: NO];
    }
    else if( [MengineAppleAppLovinPlugin_IsAgeRestrictedUser caseInsensitiveCompare:@"UNKNOWN"] == NSOrderedSame )
    {
        //Nothing
    }
    else
    {
        Mengine::Helper::AppleLog(@"🔴 [ERROR] AppLovin plugin invalid config [%@.IsAgeRestrictedUser] value %@ [YES|NO|UNKNOWN]", PLUGIN_BUNDLE_NAME, MengineAppleAppLovinPlugin_IsAgeRestrictedUser);
        
        return NO;
    }
    
    NSString * MengineAppleAppLovinPlugin_CCPA = Mengine::Helper::AppleGetBundlePluginConfigString(PLUGIN_BUNDLE_NAME, @"CCPA", @"UNKNOWN");
    
    if( [MengineAppleAppLovinPlugin_CCPA caseInsensitiveCompare:@"YES"] == NSOrderedSame )
    {
        [ALPrivacySettings setDoNotSell: YES];
    }
    else if( [MengineAppleAppLovinPlugin_CCPA caseInsensitiveCompare:@"NO"] == NSOrderedSame )
    {
        [ALPrivacySettings setDoNotSell: NO];
    }
    else if( [MengineAppleAppLovinPlugin_CCPA caseInsensitiveCompare:@"UNKNOWN"] == NSOrderedSame )
    {
        //Nothing
    }
    else
    {
        Mengine::Helper::AppleLog(@"🔴 [ERROR] AppLovin plugin invalid config [%@.CCPA] value %@ [YES|NO|UNKNOWN]", PLUGIN_BUNDLE_NAME, MengineAppleAppLovinPlugin_CCPA);
        
        return NO;
    }
    
    ALSdkSettings * settings = [[ALSdkSettings alloc] init];
    
    BOOL MengineAppleAppLovinPlugin_Verbose = Mengine::Helper::AppleGetBundlePluginConfigBoolean(PLUGIN_BUNDLE_NAME, @"Verbose", NO);
    
    if( MengineAppleAppLovinPlugin_Verbose == YES )
    {
        settings.verboseLoggingEnabled = YES;
    }
    
    BOOL MengineAppleAppLovinPlugin_TermsAndPrivacyPolicyFlow = Mengine::Helper::AppleGetBundlePluginConfigBoolean(PLUGIN_BUNDLE_NAME, @"TermsAndPrivacyPolicyFlow", NO);
    
    if( MengineAppleAppLovinPlugin_TermsAndPrivacyPolicyFlow == YES )
    {
        NSString * MengineAppleAppLovinPlugin_PrivacyPolicyURL = Mengine::Helper::AppleGetBundlePluginConfigString(PLUGIN_BUNDLE_NAME, @"PrivacyPolicyURL", nil);
        
        NSString * MengineAppleAppLovinPlugin_TermsOfServiceURL = Mengine::Helper::AppleGetBundlePluginConfigString(PLUGIN_BUNDLE_NAME, @"TermsOfServiceURL", nil);
        
        if( MengineAppleAppLovinPlugin_PrivacyPolicyURL == nil )
        {
            Mengine::Helper::AppleLog(@"🔴 [ERROR] AppLovin plugin missed required config [%@.PrivacyPolicyURL]", PLUGIN_BUNDLE_NAME);
            
            return NO;
        }
        
        if( MengineAppleAppLovinPlugin_TermsOfServiceURL == nil )
        {
            Mengine::Helper::AppleLog(@"🔴 [ERROR] AppLovin plugin missed required config [%@.TermsOfServiceURL]", PLUGIN_BUNDLE_NAME);
            
            return NO;
        }
        
        settings.termsAndPrivacyPolicyFlowSettings.enabled = YES;
        settings.termsAndPrivacyPolicyFlowSettings.privacyPolicyURL = [NSURL URLWithString: MengineAppleAppLovinPlugin_PrivacyPolicyURL];
        settings.termsAndPrivacyPolicyFlowSettings.termsOfServiceURL = [NSURL URLWithString: MengineAppleAppLovinPlugin_TermsOfServiceURL];
    }
    
    NSString * MengineAppleAppLovinPlugin_SdkKey = Mengine::Helper::AppleGetBundlePluginConfigString(PLUGIN_BUNDLE_NAME, @"SdkKey", nil);
    
    if( MengineAppleAppLovinPlugin_SdkKey == nil )
    {
        Mengine::Helper::AppleLog(@"🔴 [ERROR] AppLovin plugin missed required config [%@.SdkKey]", PLUGIN_BUNDLE_NAME);
        
        return NO;
    }
    
    ALSdk * appLovinSdk = [ALSdk sharedWithKey:MengineAppleAppLovinPlugin_SdkKey settings:settings];
    
    appLovinSdk.mediationProvider = @"max";

    [appLovinSdk initializeSdkWithCompletionHandler:^(ALSdkConfiguration *configuration) {
        ALConsentFlowUserGeography consentFlowUserGeography = configuration.consentFlowUserGeography;
        NSString * countryCode = configuration.countryCode;
        ALAppTrackingTransparencyStatus appTrackingTransparencyStatus = configuration.appTrackingTransparencyStatus;
        BOOL testModeEnabled = configuration.testModeEnabled;
        
        Mengine::Helper::AppleLog(@"[AppLovin] plugin initialize complete");
        Mengine::Helper::AppleLog(@"[AppLovin] consent flow user geography: %ld", consentFlowUserGeography);
        Mengine::Helper::AppleLog(@"[AppLovin] country code: %@", countryCode);
        Mengine::Helper::AppleLog(@"[AppLovin] app tracking transparency status: %ld", appTrackingTransparencyStatus);
        Mengine::Helper::AppleLog(@"[AppLovin] test mode enabled: %d", testModeEnabled);
        
        Mengine::Helper::activateSemaphore(STRINGIZE_STRING_LOCAL("AppLovinSdkInitialized"));
    }];
    
    AppleAppLovinApplicationDelegate.AppLovinSdk = appLovinSdk;
    
    return YES;
}

@end
