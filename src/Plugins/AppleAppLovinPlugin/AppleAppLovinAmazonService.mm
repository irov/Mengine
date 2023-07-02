#import "AppleAppLovinAmazonService.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

#import <DTBiOSSDK/DTBAds.h>

@implementation AppleAppLovinAmazonService

- (instancetype _Nonnull) init {
    const Mengine::Char * AppLovinPlugin_AmazoneAppKey = CONFIG_VALUE( "AppLovinPlugin", "AmazonAppKey", "" );
    
    MENGINE_ASSERTION_FATAL( MENGINE_STRCMP( AppLovinPlugin_AmazoneAppKey, "" ) == 0 );

    LOGGER_MESSAGE( "Amazon AppKey '%s'"
        , AppLovinPlugin_AmazoneAppKey
    );
        
    NSString * amazonAppKey = [NSString stringWithUTF8String:AppLovinPlugin_AmazoneAppKey];
        
    [[DTBAds sharedInstance] setAppKey: amazonAppKey];
    DTBAdNetworkInfo *adNetworkInfo = [[DTBAdNetworkInfo alloc] initWithNetworkName: DTBADNETWORK_MAX];
    [DTBAds sharedInstance].mraidCustomVersions = @[@"1.0", @"2.0", @"3.0"];
    [[DTBAds sharedInstance] setAdNetworkInfo: adNetworkInfo];
    [DTBAds sharedInstance].mraidPolicy = CUSTOM_MRAID;
    
    bool AppLovinPlugin_AmazonLogLevelAll = CONFIG_VALUE( "AppLovinPlugin", "AmazonLogLevelAll", false );
    
    if( AppLovinPlugin_AmazonLogLevelAll == true ) {
        [[DTBAds sharedInstance] setLogLevel:DTBLogLevelAll];
    }
    
    bool AppLovinPlugin_AmazonTestMode = CONFIG_VALUE( "AppLovinPlugin", "AmazonTestMode", false );
    
    if( AppLovinPlugin_AmazonTestMode == true ) {
        [DTBAds sharedInstance].testMode = TRUE;
    }
    
    return self;
}

- (NSString * _Nullable) getAmazonBannerSlotId {
    const Mengine::Char * AppLovinPlugin_AmazonBannerSlotId = CONFIG_VALUE( "AppLovinPlugin", "AmazonBannerSlotId", "" );

    if( MENGINE_STRCMP( AppLovinPlugin_AmazonBannerSlotId, "" ) == 0 ) {
        return nil;
    }
        
    NSString * amazonBannerSlotId = [NSString stringWithUTF8String:AppLovinPlugin_AmazonBannerSlotId];

    LOGGER_MESSAGE( "Interstitial Amazon AdUnit '%s'"
        , AppLovinPlugin_AmazonBannerSlotId
    );
    
    return amazonBannerSlotId;
}

- (NSString * _Nullable) getAmazonInterstitialSlotId {
    const Mengine::Char * AppLovinPlugin_AmazonInterstitialSlotId = CONFIG_VALUE( "AppLovinPlugin", "AmazonInterstitialSlotId", "" );
    
    if( MENGINE_STRCMP( AppLovinPlugin_AmazonInterstitialSlotId, "" ) == 0 ) {
        return nil;
    }
    
    NSString * amazonInterstitialSlotId = [NSString stringWithUTF8String:AppLovinPlugin_AmazonInterstitialSlotId];

    LOGGER_MESSAGE( "Interstitial Amazon AdUnit '%s'"
        , AppLovinPlugin_AmazonInterstitialSlotId
    );
    
    return amazonInterstitialSlotId;
}

- (NSString * _Nullable) getAmazonRewardedSlotId {
    const Mengine::Char * AppLovinPlugin_AmazonRewardedSlotId = CONFIG_VALUE( "AppLovinPlugin", "AmazonRewardedSlotId", "" );
    
    if( MENGINE_STRCMP( AppLovinPlugin_AmazonRewardedSlotId, "" ) == 0 ) {
        return nil;
    }
     
    NSString * amazonRewardedSlotId = [NSString stringWithUTF8String:AppLovinPlugin_AmazonRewardedSlotId];

    LOGGER_MESSAGE( "Rewarded Amazon AdUnit '%s'"
        , AppLovinPlugin_AmazonRewardedSlotId
    );
    
    return amazonRewardedSlotId;
}

@end
