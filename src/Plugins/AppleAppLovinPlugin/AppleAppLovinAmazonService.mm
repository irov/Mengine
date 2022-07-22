#import "AppleAppLovinAmazonService.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

#import <DTBiOSSDK/DTBAds.h>

@implementation AppleAppLovinAmazonService

- (instancetype _Nonnull) init {
    const Mengine::Char * AppLovin_AmazoneAppKey = CONFIG_VALUE("AppLovin", "AmazonAppKey", "");
    
    MENGINE_ASSERTION_FATAL( MENGINE_STRCMP( AppLovin_AmazoneAppKey, "" ) == 0 );

    LOGGER_INFO("applovin", "Amazon AppKey '%s'"
        , AppLovin_AmazoneAppKey
    );
        
    NSString * amazonAppKey = [NSString stringWithUTF8String:AppLovin_AmazoneAppKey];
        
    [[DTBAds sharedInstance] setAppKey: amazonAppKey];
    DTBAdNetworkInfo *adNetworkInfo = [[DTBAdNetworkInfo alloc] initWithNetworkName: DTBADNETWORK_MAX];
    [DTBAds sharedInstance].mraidCustomVersions = @[@"1.0", @"2.0", @"3.0"];
    [[DTBAds sharedInstance] setAdNetworkInfo: adNetworkInfo];
    [DTBAds sharedInstance].mraidPolicy = CUSTOM_MRAID;
    
    bool AppLovin_AmazonLogLevelAll = CONFIG_VALUE("AppLovin", "AmazonLogLevelAll", false);
    
    if( AppLovin_AmazonLogLevelAll == true ) {
        [[DTBAds sharedInstance] setLogLevel:DTBLogLevelAll];
    }
    
    bool AppLovin_AmazonTestMode = CONFIG_VALUE("AppLovin", "AmazonTestMode", false);
    
    if( AppLovin_AmazonTestMode == true ) {
        [DTBAds sharedInstance].testMode = TRUE;
    }
    
    return self;
}

- (NSString * _Nullable) getAmazonBannerSlotId {
    const Mengine::Char * AppLovin_AmazonBannerSlotId = CONFIG_VALUE("AppLovin", "AmazonBannerSlotId", "");

    if( MENGINE_STRCMP( AppLovin_AmazonBannerSlotId, "" ) == 0 ) {
        return nil;
    }
        
    NSString * amazonBannerSlotId = [NSString stringWithUTF8String:AppLovin_AmazonBannerSlotId];
        
    LOGGER_INFO("applovin", "Interstitial Amazon AdUnit '%s'"
        , AppLovin_AmazonBannerSlotId
    );
    
    return amazonBannerSlotId;
}

- (NSString * _Nullable) getAmazonInterstitialSlotId {
    const Mengine::Char * AppLovin_AmazonInterstitialSlotId = CONFIG_VALUE("AppLovin", "AmazonInterstitialSlotId", "");
    
    if( MENGINE_STRCMP( AppLovin_AmazonInterstitialSlotId, "" ) == 0 ) {
        return nil;
    }
    
    NSString * amazonInterstitialSlotId = [NSString stringWithUTF8String:AppLovin_AmazonInterstitialSlotId];
        
    LOGGER_INFO("applovin", "Interstitial Amazon AdUnit '%s'"
        , AppLovin_AmazonInterstitialSlotId
    );
    
    return amazonInterstitialSlotId;
}

- (NSString * _Nullable) getAmazonRewardedSlotId {
    const Mengine::Char * AppLovin_AmazonVideoRewardedSlotId = CONFIG_VALUE("AppLovin", "AmazonVideoRewardedSlotId", "");
    
    if( MENGINE_STRCMP( AppLovin_AmazonVideoRewardedSlotId, "" ) == 0 ) {
        return nil;
    }
     
    NSString * amazonRewardedSlotId = [NSString stringWithUTF8String:AppLovin_AmazonVideoRewardedSlotId];
        
    LOGGER_INFO("applovin", "Rewarded Amazon AdUnit '%s'"
        , AppLovin_AmazonVideoRewardedSlotId
    );
    
    return amazonRewardedSlotId;
}

@end
