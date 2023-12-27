#import "AppleAppLovinApplicationDelegate.h"

#include "Environment/Apple/AppleDetail.h"

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

@implementation AppleAppLovinApplicationDelegate

#pragma mark - UIPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if( Mengine::Helper::AppleHasBundlePluginConfig(@"MengineAppleAppLovinPlugin") == NO )
    {
        return NO;
    }
    
    NSString * MengineAppleAppLovinPlugin_IsAgeRestrictedUser = Mengine::Helper::AppleGetBundlePluginConfigString(@"MengineAppleAppLovinPlugin", @"IsAgeRestrictedUser", @"UNKNOWN");
    
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
        return NO;
    }
    
    NSString * MengineAppleAppLovinPlugin_CCPA = Mengine::Helper::AppleGetBundlePluginConfigString(@"MengineAppleAppLovinPlugin", @"CCPA", @"UNKNOWN");
    
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
        return NO;
    }
    
    ALSdkSettings * settings = [[ALSdkSettings alloc] init];
    
    BOOL MengineAppleAppLovinPlugin_Verbose = Mengine::Helper::AppleGetBundlePluginConfigBoolean(@"MengineAppleAppLovinPlugin", @"Verbose", NO);
    
    if( MengineAppleAppLovinPlugin_Verbose == YES )
    {
        settings.verboseLoggingEnabled = YES;
    }
    
    BOOL MengineAppleAppLovinPlugin_TermsAndPrivacyPolicyFlow = Mengine::Helper::AppleGetBundlePluginConfigBoolean(@"MengineAppleAppLovinPlugin", @"TermsAndPrivacyPolicyFlow", NO);
    
    if( MengineAppleAppLovinPlugin_TermsAndPrivacyPolicyFlow == YES )
    {
        NSString * MengineAppleAppLovinPlugin_PrivacyPolicyURL = Mengine::Helper::AppleGetBundlePluginConfigString(@"MengineAppleAppLovinPlugin", @"PrivacyPolicyURL", nil);
        
        NSString * MengineAppleAppLovinPlugin_TermsOfServiceURL = Mengine::Helper::AppleGetBundlePluginConfigString(@"MengineAppleAppLovinPlugin", @"TermsOfServiceURL", nil);
        
        if( MengineAppleAppLovinPlugin_PrivacyPolicyURL == nil || MengineAppleAppLovinPlugin_TermsOfServiceURL == nil )
        {
            return NO;
        }
        
        settings.termsAndPrivacyPolicyFlowSettings.enabled = YES;
        settings.termsAndPrivacyPolicyFlowSettings.privacyPolicyURL = [NSURL URLWithString: MengineAppleAppLovinPlugin_PrivacyPolicyURL];
        settings.termsAndPrivacyPolicyFlowSettings.termsOfServiceURL = [NSURL URLWithString: MengineAppleAppLovinPlugin_TermsOfServiceURL];
    }
    
    NSString * MengineAppleAppLovinPlugin_SdkKey = Mengine::Helper::AppleGetBundlePluginConfigString(@"MengineAppleAppLovinPlugin", @"SdkKey", nil);
    
    if( MengineAppleAppLovinPlugin_SdkKey == nil )
    {
        return NO;
    }
    
    ALSdk * appLovinSdk = [ALSdk sharedWithKey:MengineAppleAppLovinPlugin_SdkKey settings:settings];
    
    appLovinSdk.mediationProvider = @"max";

    [[ALSdk shared] initializeSdkWithCompletionHandler:^(ALSdkConfiguration *configuration) {
    }];
    
    return YES;
}

@end
