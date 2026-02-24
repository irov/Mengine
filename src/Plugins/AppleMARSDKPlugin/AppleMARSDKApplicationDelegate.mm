#import "AppleMARSDKApplicationDelegate.h"

#import "Environment/Apple/AppleDetail.h"

#include "Environment/Apple/AppleBundle.h"
#include "Environment/Apple/AppleString.h"
#include "Environment/Apple/AppleErrorHelper.h"
#include "Environment/iOS/iOSDetail.h"

#include "Kernel/Logger.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleMARSDKScriptEmbedding.h"
#endif

#import "MARSDKCore/MARSDKCore.h"
#import "MARSDKCore/MARAd.h"
#import "EPSDK/EP_AppStorePay.h"

@implementation AppleMARSDKApplicationDelegate

+ (instancetype)sharedInstance {
    static AppleMARSDKApplicationDelegate * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[AppleMARSDKApplicationDelegate alloc] init];
    });

    return sharedInstance;
}

- (instancetype)init {
    self = [super init];

    self.m_provider = nil;
    self.m_delegate = [[AppleMARSDKDelegate alloc] initWithMARSDK:self];
    self.m_adRewardedDelegate = [[AppleMARSDKAdRewardedDelegate alloc] initWithMARSDK:self];

    return self;
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (void)onFinalize {
    [[MARSDK sharedInstance] setDelegate:nil];

    self.m_provider = nil;
    self.m_delegate = nil;
    self.m_adRewardedDelegate = nil;
}

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
- (void)onRunBegin {
    Mengine::Helper::addScriptEmbedding<Mengine::AppleMARSDKScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE );
}

- (void)onStopEnd {
    Mengine::Helper::removeScriptEmbedding<Mengine::AppleMARSDKScriptEmbedding>();
}
#endif

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [[MARSDK sharedInstance] setDelegate:self.m_delegate];

    NSDictionary * MARSDKConfig = Mengine::Helper::AppleGetBundlePluginConfig(@"MARSDK");
    [[MARSDK sharedInstance] initWithParams:MARSDKConfig];

#ifdef MENGINE_DEBUG
    [[MARSDK sharedInstance] openXcodeLog];
#endif

    [[MARSDK sharedInstance] application:application didFinishLaunchingWithOptions:launchOptions];
    
    return YES;
}

#pragma mark - UISceneSession lifecycle

- (void)applicationWillResignActive:(UIApplication *)application {
    [[MARSDK sharedInstance] applicationWillResignActive:application];
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    [[MARSDK sharedInstance] applicationDidEnterBackground:application];
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    [[MARSDK sharedInstance] applicationWillEnterForeground:application];
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    [[MARSDK sharedInstance] applicationDidBecomeActive:application];
}

- (void)applicationWillTerminate:(UIApplication *)application {
    [[MARSDK sharedInstance] applicationWillTerminate:application];
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    [[MARSDK sharedInstance] application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void(^)(UIBackgroundFetchResult)) completionHandler {
    [[MARSDK sharedInstance] application:application didReceiveRemoteNotification: userInfo fetchCompletionHandler:completionHandler];
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options handled:(BOOL *)handler {
    *handler = YES;
    
    return [[MARSDK sharedInstance] application:application openURL:url options:options];
}

#pragma mark - AppleMARSDKInterface

- (void)setProvider:(id<AppleMARSDKProviderInterface>)provider {
    self.m_provider = provider;
}

- (id<AppleMARSDKProviderInterface>)getProvider {
    return self.m_provider;
}

- (BOOL)login {
    LOGGER_MESSAGE( "login" );

    [[MARSDK sharedInstance] login];

    return YES;
}

- (BOOL)logout {
    LOGGER_MESSAGE( "logout" );

    [[MARSDK sharedInstance] logout];

    return YES;
}

- (BOOL)switchAccount {
    LOGGER_MESSAGE( "switchAccount" );

    [[MARSDK sharedInstance] switchAccount];

    return YES;
}

- (void)requestNonConsumablePurchased {
    LOGGER_MESSAGE( "requestNonConsumablePurchased" );

    id<AppleMARSDKProviderInterface> provider = self.m_provider;

    [[EP_AppStorePay sharedInstance] setPurchasedNonConsumableArray:^(NSArray * _Nonnull purchasedNonConsumableArray) {
        LOGGER_MESSAGE( "onPurchasedNonConsumable: %s"
            , [[NSString stringWithFormat:@"%@", purchasedNonConsumableArray] UTF8String]
        );

        if( provider == nil )
        {
            LOGGER_ERROR( "not setup MARSDK provider" );

            return;
        }

        [AppleDetail addMainQueueOperation:^{
            [provider onPurchasedNonConsumable:purchasedNonConsumableArray];
        }];
    }];
}

- (void)submitExtendedData:(NSString *)data {
    LOGGER_MESSAGE( "submitExtendedData data: %s"
        , [data UTF8String]
    );

    MARUserExtraData * extraData = [MARUserExtraData dataFromJsonString:data];

    [[MARSDK sharedInstance] submitExtraData:extraData];
}

- (void)submitPaymentData:(NSString *)data {
    LOGGER_MESSAGE( "submitPaymentData data: %s"
        , [data UTF8String]
    );

    MARProductInfo * productInfo = [MARProductInfo productFromJsonString:data];

    if( productInfo == nil )
    {
        LOGGER_ERROR( "invalid create product from data: %s"
            , [data UTF8String]
        );

        return;
    }

    [[MARSDK sharedInstance] pay:productInfo];
}

- (void)propComplete:(NSString *)productId {
    LOGGER_MESSAGE( "propComplete orderId: %s"
        , [productId UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = self.m_provider;
    NSString * orderId = [productId copy];

    [[MARSDK sharedInstance] propComplete:orderId
                          responseHandler:^(NSURLResponse * _response, id _data, NSError * _connectionError) {
        (void)_response;

        if( _connectionError != nil )
        {
            LOGGER_ERROR( "prop complete '%s' response failed data: %s error: %s"
                , [orderId UTF8String]
                , Mengine::Helper::NSIdToString( _data ).c_str()
                , Mengine::Helper::AppleGetMessageFromNSError( _connectionError ).c_str()
            );

            if( provider == nil )
            {
                LOGGER_ERROR( "not setup MARSDK provider" );

                return;
            }

            [AppleDetail addMainQueueOperation:^{
                [provider onPropError:orderId];
            }];

            return;
        }

        LOGGER_MESSAGE( "prop complete '%s' response successful data: %s"
            , [orderId UTF8String]
            , Mengine::Helper::NSIdToString( _data ).c_str()
        );

        if( provider == nil )
        {
            LOGGER_ERROR( "not setup MARSDK provider" );

            return;
        }

        [AppleDetail addMainQueueOperation:^{
            [provider onPropComplete:orderId];
        }];
    }];
}

- (void)showRewardVideoAd:(NSString *)itemName itemNum:(NSUInteger)itemNum {
    LOGGER_MESSAGE( "showRewardVideoAd itemName: %s itemNum: %u"
        , [itemName UTF8String]
        , (uint32_t)itemNum
    );

    UIViewController * rootViewController = Mengine::Helper::iOSGetRootViewController();

    [[MARAd sharedInstance] showRewardVideoAd:rootViewController
                                     itemName:itemName
                                      itemNum:(int)itemNum
                                     delegate:self.m_adRewardedDelegate];
}

- (NSInteger)getInternetDate {
    NSInteger date = [[MARSDK sharedInstance] getInternetDate];

    return date;
}

@end
