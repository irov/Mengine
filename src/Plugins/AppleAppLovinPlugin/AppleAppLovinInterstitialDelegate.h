#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

@interface AppLovinInterstitialDelegate : NSObject<MAAdDelegate>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString* _Nonnull) key;

- (BOOL) hasLoaded;
- (BOOL) show;

@property (nonatomic, strong) MAInterstitialAd *interstitialAd;
@property (nonatomic, assign) NSInteger retryAttempt;
@end
