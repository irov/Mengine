#import "AppleAdvertisementPlugin.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/SoundServiceInterface.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleSemaphoreService.h"
#import "Environment/Apple/AppleString.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleAdvertisementScriptEmbedding.h"
#endif

#include "Kernel/ConstStringHelper.h"
#include "Kernel/ScriptEmbeddingHelper.h"

@implementation AppleAdvertisementPlugin

- (instancetype)init {
    self = [super init];
    
    self.m_optionNoAds = [AppleDetail hasOption:@"ad.no_ads"];
    self.m_noAds = NO;
    
    self.m_lastShowInterstitial = -1;
    self.m_lastShowRewarded = -1;

    self.m_countShowInterstitial = 0;
    self.m_countShowRewarded = 0;
    
    self.m_attempts = [NSMutableDictionary dictionary];
    self.m_cooldowns = [NSMutableDictionary dictionary];
    
    self.m_interstitialPoints = nil;
    self.m_rewardedPoints = nil;
    
    return self;
}

#pragma mark - AppleAdvertisementInterface

+ (instancetype)sharedInstance {
    static AppleAdvertisementPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleAdvertisementPlugin class]];
    });
    return sharedInstance;
}

- (BOOL)getNoAds {
    if (self.m_optionNoAds == YES) {
        return YES;
    }

    if (self.m_noAds == NO) {
        return NO;
    }
     
    return YES;
}

- (void)setProvider:(id<AppleAdvertisementProviderInterface>)provider {
    self.m_provider = provider;
}

- (id<AppleAdvertisementProviderInterface>)getProvider {
    return self.m_provider;
}

- (void)readyAdProvider {
    [AppleSemaphoreService.sharedInstance activateSemaphore:@"AdServiceReady"];
}

- (void)setBannerCallback:(id<AppleAdvertisementCallbackInterface>)callback {
    self.m_bannerCallback = callback;
}

- (id<AppleAdvertisementCallbackInterface>)getBannerCallback {
    return self.m_bannerCallback;
}

- (void)setInterstitialCallback:(id<AppleAdvertisementCallbackInterface>)callback {
    self.m_interstitialCallback = callback;
}

- (id<AppleAdvertisementCallbackInterface>)getInterstitialCallback {
    return self.m_interstitialCallback;
}

- (void)setRewardedCallback:(id<AppleAdvertisementCallbackInterface>)callback {
    self.m_rewardedCallback = callback;
}

- (id<AppleAdvertisementCallbackInterface>)getRewardedCallback {
    return self.m_rewardedCallback;
}

- (void)setAdFreeze:(NSString *)adName freeze:(BOOL)freeze {
    Mengine::ConstString adNameConst = [AppleString NSStringToConstString:adName];
    
    APPLICATION_SERVICE()
        ->setUpdateFreeze( adNameConst, freeze );
    
    APPLICATION_SERVICE()
        ->setRenderFreeze( adNameConst, freeze );
    
    SOUND_SERVICE()
        ->setMute( adNameConst, freeze );
}

#pragma mark - AppleAdvertisementProviderInterface

- (BOOL)hasBanner {
    if (self.m_provider == nil) {
        return NO;
    }
    
    BOOL noAds = [self getNoAds];
    
    if (noAds == YES) {
        return NO;
    }
    
    if ([self.m_provider hasBanner] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)showBanner {
    if (self.m_provider == nil) {
        return NO;
    }
    
    BOOL noAds = [self getNoAds];
    
    if (noAds == YES) {
        return NO;
    }
    
    if ([self.m_provider showBanner] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)hideBanner {
    if (self.m_provider == nil) {
        return NO;
    }
    
    BOOL noAds = [self getNoAds];
    
    if (noAds == YES) {
        return NO;
    }
    
    if ([self.m_provider hideBanner] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)getBannerWidth:(uint32_t *)width height:(uint32_t *)height {
    if (self.m_provider == nil) {
        return NO;
    }
    
    BOOL noAds = [self getNoAds];
    
    if (noAds == YES) {
        return NO;
    }
    
    if ([self.m_provider getBannerWidth:width height:height] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)hasInterstitial {
    if (self.m_provider == nil) {
        return NO;
    }
    
    BOOL noAds = [self getNoAds];
    
    if (noAds == YES) {
        return NO;
    }
    
    if ([self.m_provider hasInterstitial] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)canYouShowInterstitial:(NSString *)placement {
    AppleAdvertisementInterstitialPoint * adPoint = [self getAdInterstitialPoint:placement];
    
    if (adPoint == nil) {
        IOS_LOGGER_ERROR(@"interstitial ad point '%@' not found", placement);
        
        return NO;
    }
    
    if (self.m_provider == nil) {
        return NO;
    }
    
    BOOL noAds = [self getNoAds];
    
    if (noAds == YES) {
        return NO;
    }
    
    if ([adPoint canYouShowAd] == NO) {
        return NO;
    }
    
    if ([self.m_provider canYouShowInterstitial:placement] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)showInterstitial:(NSString *)placement {
    AppleAdvertisementInterstitialPoint * adPoint = [self getAdInterstitialPoint:placement];
    
    if (adPoint == nil) {
        IOS_LOGGER_ERROR(@"interstitial ad point '%@' not found", placement);
        
        return NO;
    }
    
    if (self.m_provider == nil) {
        return NO;
    }
    
    BOOL noAds = [self getNoAds];
    
    if (noAds == YES) {
        return NO;
    }
    
    if ([self.m_provider showInterstitial:placement] == NO) {
        return NO;
    }
    
    self.m_lastShowInterstitial = [AppleDetail getTimestamp];
    self.m_countShowInterstitial += 1;
    
    [adPoint showAd];
    
    return YES;
}

- (BOOL)isShowingInterstitial {
    if (self.m_provider == nil) {
        return NO;
    }
    
    BOOL noAds = [self getNoAds];
    
    if (noAds == YES) {
        return NO;
    }
    
    if ([self.m_provider isShowingInterstitial] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)hasRewarded {
    if (self.m_provider == nil) {
        return NO;
    }
    
    if ([self.m_provider hasRewarded] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)canOfferRewarded:(NSString *)placement {
    AppleAdvertisementRewardedPoint * adPoint = [self getAdRewardedPoint:placement];
    
    if (adPoint == nil) {
        IOS_LOGGER_ERROR(@"rewarded ad point '%@' not found", placement);
        
        return NO;
    }
    
    if (self.m_provider == nil) {
        return NO;
    }
    
    if ([adPoint canOfferAd] == NO) {
        return NO;
    }
    
    if ([self.m_provider canOfferRewarded:placement] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)canYouShowRewarded:(NSString *)placement {
    AppleAdvertisementRewardedPoint * adPoint = [self getAdRewardedPoint:placement];
    
    if (adPoint == nil) {
        IOS_LOGGER_ERROR(@"rewarded ad point '%@' not found", placement);
        
        return NO;
    }
    
    if (self.m_provider == nil) {
        return NO;
    }
    
    if ([adPoint canYouShowAd] == NO) {
        return NO;
    }
    
    if ([self.m_provider canYouShowRewarded:placement] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)showRewarded:(NSString *)placement {
    AppleAdvertisementRewardedPoint * adPoint = [self getAdRewardedPoint:placement];
    
    if (adPoint == nil) {
        IOS_LOGGER_ERROR(@"rewarded ad point '%@' not found", placement);
        
        return NO;
    }
    
    if (self.m_provider == nil) {
        return NO;
    }
    
    if ([self.m_provider showRewarded:placement] == NO) {
        return NO;
    }
    
    self.m_lastShowRewarded = [AppleDetail getTimestamp];
    self.m_countShowRewarded += 1;
    
    [adPoint showAd];
    
    return YES;
}

- (BOOL)isShowingRewarded {
    if (self.m_provider == nil) {
        return NO;
    }
    
    if ([self.m_provider isShowingRewarded] == NO) {
        return NO;
    }
    
    return YES;
}

#pragma mark - iOSPluginConfigDelegateInterface

- (AppleAdvertisementInterstitialPoint *)getAdInterstitialPoint:(NSString *)placement {
    @synchronized (self) {
        return [self.m_interstitialPoints objectForKey:placement];
    }
}

- (AppleAdvertisementRewardedPoint *)getAdRewardedPoint:(NSString *)placement {
    @synchronized (self) {
        return [self.m_rewardedPoints objectForKey:placement];
    }
}

- (void)setupAdBasePointAttemts:(AppleAdvertisementBasePoint *)point {
    NSString * name = [point getName];
    
    AppleAdvertisementAttempts * attempts = [self.m_attempts objectForKey:name];
    
    if (attempts == nil) {
        attempts = [[AppleAdvertisementAttempts alloc] init];
        
        [self.m_attempts setObject:attempts forKey:name];
    }
    
    [point setAttempts:attempts];
}

- (void)setupAdBasePointCooldown:(AppleAdvertisementBasePoint *)point {
    NSString * cooldownGroupName = [point getCooldownGroupName];
    
    if (cooldownGroupName == nil) {
        AppleAdvertisementCooldown * newCooldown = [[AppleAdvertisementCooldown alloc] init];
        
        [point setCooldown:newCooldown];
    } else {
        AppleAdvertisementCooldown * cooldown = [self.m_cooldowns objectForKey:cooldownGroupName];
        
        if (cooldown == nil) {
            cooldown = [[AppleAdvertisementCooldown alloc] init];
            
            [self.m_cooldowns setObject:cooldown forKey:cooldownGroupName];
        }
        
        [point setCooldown:cooldown];
    }
}

- (void)parseAdInterstitialAdPoint:(NSMutableDictionary *)points withName:(NSString *)name withJson:(NSDictionary *)json {
    if ([points objectForKey:name] != nil) {
        IOS_LOGGER_ERROR(@"already exist interstitial ad point '%@'", name);
        
        return;
    }
    
    AppleAdvertisementInterstitialPoint * point = [[AppleAdvertisementInterstitialPoint alloc] initWithName:name withJson:json];
    
    [self setupAdBasePointCooldown:point];
    [self setupAdBasePointAttemts:point];
    
    [points setObject:point forKey:name];
}

- (void)parseAdRewardedAdPoint:(NSMutableDictionary *)points withName:(NSString *)name withJson:(NSDictionary *)json {
    if ([points objectForKey:name] != nil) {
        IOS_LOGGER_ERROR(@"already exist rewarded ad point '%@'", name);
        
        return;
    }
    
    AppleAdvertisementRewardedPoint * point = [[AppleAdvertisementRewardedPoint alloc] initWithName:name withJson:json];
    
    [self setupAdBasePointCooldown:point];
    [self setupAdBasePointAttemts:point];
    
    [points setObject:point forKey:name];
}

- (void)onConfig:(NSDictionary * _Nonnull)config ids:(NSDictionary * _Nonnull)ids {
    @synchronized (self) {
        NSMutableDictionary * interstitialPoints = [NSMutableDictionary dictionary];
        NSMutableDictionary * rewardedPoints = [NSMutableDictionary dictionary];
        
        NSString * adInterstitialPrefix = @"ad_interstitial_";
        NSString * adRewardedPrefix = @"ad_rewarded_";
        
        for (NSString * key in config) {
            id adPointConfig = [config objectForKey:key];
            
            if ([key hasPrefix:adInterstitialPrefix]) {
                NSString * adPointName = [key substringFromIndex:adInterstitialPrefix.length];
                
                [self parseAdInterstitialAdPoint:interstitialPoints withName:adPointName withJson:adPointConfig];
            } else if ([key hasPrefix:adRewardedPrefix]) {
                NSString * adPointName = [key substringFromIndex:adRewardedPrefix.length];
                
                [self parseAdRewardedAdPoint:rewardedPoints withName:adPointName withJson:adPointConfig];
            }
        }
        
        self.m_interstitialPoints = interstitialPoints;
        self.m_rewardedPoints = rewardedPoints;
    }
}

#pragma mark - iOSPluginInterface

- (void)onRunBegin {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::AppleAdvertisementScriptEmbedding>( MENGINE_DOCUMENT_FUNCTION );
#endif
}

- (void)onStopEnd {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::AppleAdvertisementScriptEmbedding>();
#endif
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    return YES;
}

@end
