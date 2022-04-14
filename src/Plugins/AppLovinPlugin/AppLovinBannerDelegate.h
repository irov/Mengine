#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

@interface AppLovinBannerDelegate : NSObject<MAAdViewAdDelegate>

- (instancetype _Nonnull) initWithRect:(CGRect) bannerRect key:(NSString* _Nonnull) key;

- (void) show;
- (void) hide;

- (UIViewController* _Nullable) rootViewConstroller;

@property (nonatomic, strong) MAAdView *adView;
@end
