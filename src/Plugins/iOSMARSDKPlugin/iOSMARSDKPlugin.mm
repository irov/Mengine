#import "iOSMARSDKPlugin.h"

#import "Environment/Apple/AppleDetail.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleString.h"
#import "Environment/Apple/AppleErrorHelper.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"


#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "iOSMARSDKScriptEmbedding.h"
#endif

#import "MARSDKCore/MARSDKCore.h"
#import "MARSDKCore/MARAd.h"
#import "EPSDK/EP_AppStorePay.h"

@implementation iOSMARSDKPlugin

+ (instancetype)sharedInstance {
    static iOSMARSDKPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[iOSMARSDKPlugin alloc] init];
    });

    return sharedInstance;
}

- (instancetype)init {
    self = [super init];

    self.m_provider = nil;
    self.m_delegate = [[iOSMARSDKDelegate alloc] initWithMARSDK:self];
    self.m_adRewardedDelegate = [[iOSMARSDKAdRewardedDelegate alloc] initWithMARSDK:self];

    return self;
}

#pragma mark - iOSPluginInterface

- (void)onFinalize {
    [[MARSDK sharedInstance] setDelegate:nil];

    self.m_provider = nil;
    self.m_delegate = nil;
    self.m_adRewardedDelegate = nil;
}

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
- (void)onRunBegin {
    Mengine::Helper::addScriptEmbedding<Mengine::iOSMARSDKScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE );
}

- (void)onStopEnd {
    Mengine::Helper::removeScriptEmbedding<Mengine::iOSMARSDKScriptEmbedding>();
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

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options {
    BOOL result = [[MARSDK sharedInstance] application:application openURL:url options:options];

    return result;
}

#pragma mark - iOSMARSDKInterface

- (void)setProvider:(id<iOSMARSDKProviderInterface>)provider {
    self.m_provider = provider;
}

- (id<iOSMARSDKProviderInterface>)getProvider {
    return self.m_provider;
}

- (BOOL)login {
    IOS_LOGGER_MESSAGE( @"login" );

    [[MARSDK sharedInstance] login];

    return YES;
}

- (BOOL)logout {
    IOS_LOGGER_MESSAGE( @"logout" );

    [[MARSDK sharedInstance] logout];

    return YES;
}

- (BOOL)switchAccount {
    IOS_LOGGER_MESSAGE( @"switchAccount" );

    [[MARSDK sharedInstance] switchAccount];

    return YES;
}

- (void)requestNonConsumablePurchased {
    IOS_LOGGER_MESSAGE( @"requestNonConsumablePurchased" );

    id<iOSMARSDKProviderInterface> provider = self.m_provider;

    [[EP_AppStorePay sharedInstance] setPurchasedNonConsumableArray:^(NSArray * _Nonnull purchasedNonConsumableArray) {
        IOS_LOGGER_MESSAGE( @"onPurchasedNonConsumable: %@"
            , purchasedNonConsumableArray
        );

        if( provider == nil )
        {
            IOS_LOGGER_ERROR( @"not setup MARSDK provider" );

            return;
        }

        [AppleDetail addMainQueueOperation:^{
            [provider onPurchasedNonConsumable:purchasedNonConsumableArray];
        }];
    }];
}

- (void)submitExtendedData:(NSString *)data {
    IOS_LOGGER_MESSAGE( @"submitExtendedData data: %@"
        , data
    );

    MARUserExtraData * extraData = [MARUserExtraData dataFromJsonString:data];

    [[MARSDK sharedInstance] submitExtraData:extraData];
}

- (void)submitPaymentData:(NSString *)data {
    IOS_LOGGER_MESSAGE( @"submitPaymentData data: %@"
        , data
    );

    MARProductInfo * productInfo = [MARProductInfo productFromJsonString:data];

    if( productInfo == nil )
    {
        IOS_LOGGER_ERROR( @"invalid create product from data: %@"
            , data
        );

        return;
    }

    [[MARSDK sharedInstance] pay:productInfo];
}

- (void)propComplete:(NSString *)productId {
    IOS_LOGGER_MESSAGE( @"propComplete orderId: %@"
        , productId
    );

    id<iOSMARSDKProviderInterface> provider = self.m_provider;
    NSString * orderId = [productId copy];

    [[MARSDK sharedInstance] propComplete:orderId
                          responseHandler:^(NSURLResponse * _response, id _data, NSError * _connectionError) {
        (void)_response;

        if( _connectionError != nil )
        {
            IOS_LOGGER_ERROR( @"prop complete '%@' response failed data: %@ error: %@"
                , orderId
                , [AppleDetail NSIdToString:_data]
                , [AppleDetail getMessageFromNSError:_connectionError]
            );

            if( provider == nil )
            {
                IOS_LOGGER_ERROR( @"not setup MARSDK provider" );

                return;
            }

            [AppleDetail addMainQueueOperation:^{
                [provider onPropError:orderId];
            }];

            return;
        }

        IOS_LOGGER_MESSAGE( @"prop complete '%@' response successful data: %@"
            , orderId
            , _data
        );

        if( provider == nil )
        {
            IOS_LOGGER_ERROR( @"not setup MARSDK provider" );

            return;
        }

        [AppleDetail addMainQueueOperation:^{
            [provider onPropComplete:orderId];
        }];
    }];
}

- (void)showRewardVideoAd:(NSString *)itemName itemNum:(NSUInteger)itemNum {
    IOS_LOGGER_MESSAGE( @"showRewardVideoAd itemName: %@ itemNum: %u"
        , itemName
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
