#import "AppleAppLovinFirebaseAnalytics.h"

#import "Plugins/AppleFirebaseAnalyticsPlugin/AppleFirebaseAnalyticsInterface.h"

@implementation AppleAppLovinFirebaseAnalytics

- (void) eventRevenuePaid:(MAAd * _Nonnull) ad {
    NSString * kFIREventAdImpression = APPLE_FIREBASE_ANALYTICS_SERVICE()
        ->getkFIREventAdImpression();
    
    NSString * kFIRParameterAdPlatform = APPLE_FIREBASE_ANALYTICS_SERVICE()
        ->getkFIRParameterAdPlatform();
        
    NSString * kFIRParameterAdSource = APPLE_FIREBASE_ANALYTICS_SERVICE()
        ->getkFIRParameterAdSource();
    
    NSString * kFIRParameterAdFormat = APPLE_FIREBASE_ANALYTICS_SERVICE()
        ->getkFIRParameterAdFormat();
    
    NSString * kFIRParameterAdUnitName = APPLE_FIREBASE_ANALYTICS_SERVICE()
        ->getkFIRParameterAdUnitName();
    
    NSString * kFIRParameterCurrency = APPLE_FIREBASE_ANALYTICS_SERVICE()
        ->getkFIRParameterCurrency();
    
    NSString * kFIRParameterValue = APPLE_FIREBASE_ANALYTICS_SERVICE()
        ->getkFIRParameterValue();
    
    APPLE_FIREBASE_ANALYTICS_SERVICE()
        ->sendEvent( kFIREventAdImpression, @{
                        kFIRParameterAdPlatform: @"AppLovin",
                        kFIRParameterAdSource: ad.networkName,
                        kFIRParameterAdFormat: ad.format.label,
                        kFIRParameterAdUnitName: ad.adUnitIdentifier,
                        kFIRParameterCurrency: @"USD", // All Applovin revenue is sent in USD
                        kFIRParameterValue: [NSNumber numberWithDouble:ad.revenue]
                    });
}

@end
