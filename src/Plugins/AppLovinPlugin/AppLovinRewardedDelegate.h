#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>


@protocol AppLovinRewardCallbackProtocol <NSObject>

- (void) receivedReward:(int) amount;

@end



@interface AppLovinRewardedDelegate : NSObject<MARewardedAdDelegate>

- (instancetype _Nonnull) initWithCallback:(id<AppLovinRewardCallbackProtocol> * _Nonnull)callback WithAdUnitIdentifier:(NSString* _Nonnull) key;

- (BOOL) hasLoaded;
- (BOOL) show;


@property (assign) id<AppLovinRewardCallbackProtocol> * _Nonnull m_callbacks;
@property (nonatomic, strong) MARewardedAd *_Nonnull rewardedAd;
@property (nonatomic, assign) NSInteger retryAttempt;
@end
