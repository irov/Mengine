#import "iOSAdvertisementPlugin.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/SoundServiceInterface.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleSemaphoreService.h"
#import "Environment/Apple/AppleString.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "iOSAdvertisementScriptEmbedding.h"
#endif

#include "Kernel/ConstStringHelper.h"
#include "Kernel/ScriptEmbeddingHelper.h"

@implementation iOSAdvertisementPlugin

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

#pragma mark - iOSAdvertisementInterface

+ (instancetype)sharedInstance {
    static iOSAdvertisementPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[iOSAdvertisementPlugin class]];
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

- (void)setProvider:(id<iOSAdvertisementProviderInterface>)provider {
    [self.m_provider hideBanner];
    [self.m_provider hideTopper];
    self.m_provider = provider;
}

- (id<iOSAdvertisementProviderInterface>)getProvider {
    return self.m_provider;
}

- (void)readyAdProvider {
    [AppleSemaphoreService.sharedInstance activateSemaphore:@"AdServiceReady"];
}

- (void)setBannerCallback:(id<iOSAdvertisementCallbackInterface>)callback {
    self.m_bannerCallback = callback;
}

- (id<iOSAdvertisementCallbackInterface>)getBannerCallback {
    return self.m_bannerCallback;
}

- (void)setInterstitialCallback:(id<iOSAdvertisementCallbackInterface>)callback {
    self.m_interstitialCallback = callback;
}

- (id<iOSAdvertisementCallbackInterface>)getInterstitialCallback {
    return self.m_interstitialCallback;
}

- (void)setRewardedCallback:(id<iOSAdvertisementCallbackInterface>)callback {
    self.m_rewardedCallback = callback;
}

- (id<iOSAdvertisementCallbackInterface>)getRewardedCallback {
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

#pragma mark - iOSAdvertisementProviderInterface

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

- (BOOL)isBannerLoaded {
    if (self.m_provider == nil) {
        return NO;
    }

    if ([self getNoAds] == YES) {
        return NO;
    }

    BOOL loaded = [self.m_provider isBannerLoaded];

    return loaded;
}

- (void)showBanner {
    if (self.m_provider == nil) {
        return;
    }

    BOOL noAds = [self getNoAds];

    if (noAds == YES) {
        return;
    }

    [self.m_provider showBanner];
}

- (void)hideBanner {
    if (self.m_provider == nil) {
        return;
    }

    [self.m_provider hideBanner];
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

- (BOOL)hasTopper {
    if (self.m_provider == nil) {
        return NO;
    }

    BOOL noAds = [self getNoAds];

    if (noAds == YES) {
        return NO;
    }

    if ([self.m_provider hasTopper] == NO) {
        return NO;
    }

    return YES;
}

- (BOOL)isTopperLoaded {
    if (self.m_provider == nil) {
        return NO;
    }

    if ([self getNoAds] == YES) {
        return NO;
    }

    BOOL loaded = [self.m_provider isTopperLoaded];

    return loaded;
}

- (void)showTopper {
    if (self.m_provider == nil) {
        return;
    }

    BOOL noAds = [self getNoAds];

    if (noAds == YES) {
        return;
    }

    [self.m_provider showTopper];
}

- (void)hideTopper {
    if (self.m_provider == nil) {
        return;
    }

    [self.m_provider hideTopper];
}

- (BOOL)getTopperWidth:(uint32_t *)width height:(uint32_t *)height {
    if (self.m_provider == nil) {
        return NO;
    }

    BOOL noAds = [self getNoAds];

    if (noAds == YES) {
        return NO;
    }

    if ([self.m_provider getTopperWidth:width height:height] == NO) {
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
    if ([self isShowingFullscreenAd_] == YES) {
        return NO;
    }

    iOSAdvertisementInterstitialPoint * adPoint = [self getAdInterstitialPoint:placement];

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
    if ([self isShowingFullscreenAd_] == YES) {
        return NO;
    }

    iOSAdvertisementInterstitialPoint * adPoint = [self getAdInterstitialPoint:placement];

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

- (BOOL)isShowingFullscreenAd_ {
    if ([self.m_provider isShowingInterstitial] == YES) {
        return YES;
    }

    if ([self.m_provider isShowingRewarded] == YES) {
        return YES;
    }

    if ([self.m_provider isShowingRewardedInterstitial] == YES) {
        return YES;
    }

    return NO;
}

- (BOOL)canOfferRewarded:(NSString *)placement {
    iOSAdvertisementRewardedPoint * adPoint = [self getAdRewardedPoint:placement];

    if (self.m_provider == nil) {
        return NO;
    }

    if (adPoint == nil) {
        return NO;
    }

    if ([adPoint canOfferAd] == NO) {
        return NO;
    }

    if ([self isShowingFullscreenAd_] == YES) {
        return NO;
    }

    BOOL canOffer = [self.m_provider canOfferRewarded:placement];

    return canOffer;
}

- (BOOL)canYouShowRewarded:(NSString *)placement {
    iOSAdvertisementRewardedPoint * adPoint = [self getAdRewardedPoint:placement];

    if (self.m_provider == nil) {
        return NO;
    }

    if (adPoint == nil) {
        return NO;
    }

    if ([self isShowingFullscreenAd_] == YES) {
        return NO;
    }

    if ([adPoint canYouShowAd] == NO) {
        return NO;
    }

    BOOL canShow = [self.m_provider canYouShowRewarded:placement];

    return canShow;
}

- (BOOL)showRewarded:(NSString *)placement {
    iOSAdvertisementRewardedPoint * adPoint = [self getAdRewardedPoint:placement];

    if (self.m_provider == nil) {
        return NO;
    }

    if (adPoint == nil) {
        return NO;
    }

    if ([adPoint canOfferAd] == NO) {
        return NO;
    }

    if ([self isShowingFullscreenAd_] == YES) {
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

    if ([self.m_provider isShowingRewarded] == YES) {
        return YES;
    }

    if ([self.m_provider isShowingRewardedInterstitial] == YES) {
        return YES;
    }

    return NO;
}

- (BOOL)hasRewardedInterstitial {
    BOOL hasRewardedInterstitial = [self.m_provider hasRewardedInterstitial];

    return hasRewardedInterstitial;
}

- (BOOL)canYouShowRewardedInterstitial:(NSString *)placement {
    if ([self isShowingFullscreenAd_] == YES) {
        return NO;
    }

    iOSAdvertisementRewardedPoint * adPoint = [self getAdRewardedPoint:placement];

    if (adPoint == nil) {
        IOS_LOGGER_ERROR(@"rewarded interstitial ad point '%@' not found", placement);
        return NO;
    }

    if ([adPoint canYouShowAd] == NO) {
        return NO;
    }

    BOOL canShow = [self.m_provider canYouShowRewardedInterstitial:placement];

    return canShow;
}

- (BOOL)showRewardedInterstitial:(NSString *)placement {
    if ([self isShowingFullscreenAd_] == YES) {
        return NO;
    }

    iOSAdvertisementRewardedPoint * adPoint = [self getAdRewardedPoint:placement];

    if (adPoint == nil) {
        return NO;
    }

    if (self.m_provider == nil) {
        return NO;
    }

    if ([self.m_provider showRewardedInterstitial:placement] == NO) {
        return NO;
    }

    self.m_lastShowRewarded = [AppleDetail getTimestamp];
    self.m_countShowRewarded += 1;

    [adPoint showAd];

    return YES;
}

- (BOOL)isShowingRewardedInterstitial {
    BOOL showing = [self.m_provider isShowingRewardedInterstitial];

    return showing;
}

#pragma mark - iOSPluginConfigDelegateInterface

- (iOSAdvertisementInterstitialPoint *)getAdInterstitialPoint:(NSString *)placement {
    @synchronized (self) {
        return [self.m_interstitialPoints objectForKey:placement];
    }
}

- (iOSAdvertisementRewardedPoint *)getAdRewardedPoint:(NSString *)placement {
    @synchronized (self) {
        return [self.m_rewardedPoints objectForKey:placement];
    }
}

- (void)setupAdBasePointAttemts:(iOSAdvertisementBasePoint *)point {
    NSString * name = [point getName];

    iOSAdvertisementAttempts * attempts = [self.m_attempts objectForKey:name];

    if (attempts == nil) {
        attempts = [[iOSAdvertisementAttempts alloc] init];

        [self.m_attempts setObject:attempts forKey:name];
    }

    [point setAttempts:attempts];
}

- (void)setupAdBasePointCooldown:(iOSAdvertisementBasePoint *)point {
    NSString * cooldownGroupName = [point getCooldownGroupName];

    if (cooldownGroupName == nil) {
        iOSAdvertisementCooldown * newCooldown = [[iOSAdvertisementCooldown alloc] init];

        [point setCooldown:newCooldown];
    } else {
        iOSAdvertisementCooldown * cooldown = [self.m_cooldowns objectForKey:cooldownGroupName];

        if (cooldown == nil) {
            cooldown = [[iOSAdvertisementCooldown alloc] init];

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

    iOSAdvertisementInterstitialPoint * point = [[iOSAdvertisementInterstitialPoint alloc] initWithName:name withJson:json];

    [self setupAdBasePointCooldown:point];
    [self setupAdBasePointAttemts:point];

    [points setObject:point forKey:name];
}

- (void)parseAdRewardedAdPoint:(NSMutableDictionary *)points withName:(NSString *)name withJson:(NSDictionary *)json {
    if ([points objectForKey:name] != nil) {
        IOS_LOGGER_ERROR(@"already exist rewarded ad point '%@'", name);

        return;
    }

    iOSAdvertisementRewardedPoint * point = [[iOSAdvertisementRewardedPoint alloc] initWithName:name withJson:json];

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
    Mengine::Helper::addScriptEmbedding<Mengine::iOSAdvertisementScriptEmbedding>( MENGINE_DOCUMENT_FUNCTION );
#endif
}

- (void)onStopEnd {
    [self setProvider:nil];
    self.m_bannerCallback = nil;
    self.m_interstitialCallback = nil;
    self.m_rewardedCallback = nil;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::iOSAdvertisementScriptEmbedding>();
#endif
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    return YES;
}

@end
