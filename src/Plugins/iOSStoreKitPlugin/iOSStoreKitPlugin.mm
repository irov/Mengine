#import "iOSStoreKitPlugin.h"

#import "iOSStoreKitEntitlements.h"
#import "iOSStoreKitPlugin-Swift.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Kernel/ScriptEmbeddingHelper.h"

#   include "iOSStoreKitScriptEmbedding.h"
#endif

@interface iOSStoreKitPlugin () <MengineStoreKitServiceDelegate>

@property (nonatomic, strong) MengineStoreKitService * m_service;
@property (nonatomic, strong) id<iOSStoreKitProviderInterface> m_provider;

@end

@implementation iOSStoreKitPlugin

+ (instancetype)sharedInstance {
    static iOSStoreKitPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once( &onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:iOSStoreKitPlugin.class];
    } );

    return sharedInstance;
}

- (instancetype)init {
    self = [super init];

    if (self == nil) {
        return nil;
    }

    self.m_service = [[MengineStoreKitService alloc] init];
    self.m_service.delegate = self;

    return self;
}

- (void)performProvider:(void (^)(id<iOSStoreKitProviderInterface> provider))block {
    id<iOSStoreKitProviderInterface> provider = self.m_provider;

    if (provider == nil) {
        return;
    }

    __weak iOSStoreKitPlugin * weakSelf = self;
    __weak id<iOSStoreKitProviderInterface> weakProvider = provider;

    [AppleDetail addMainQueueOperation:^{
        iOSStoreKitPlugin * strongSelf = weakSelf;
        id<iOSStoreKitProviderInterface> currentProvider = weakProvider;

        if (strongSelf == nil || currentProvider == nil) {
            return;
        }

        if (strongSelf.m_provider != currentProvider) {
            return;
        }

        block(currentProvider);
    }];
}

#pragma mark - iOSPluginInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    MENGINE_UNUSED( application );
    MENGINE_UNUSED( launchOptions );

    return YES;
}

- (void)onRunBegin {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::iOSStoreKitScriptEmbedding>( MENGINE_DOCUMENT_FUNCTION );
#endif
}

- (void)onStopEnd {
    [self setProvider:nil];

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::iOSStoreKitScriptEmbedding>();
#endif
}

- (void)onFinalize {
    [self setProvider:nil];
}

#pragma mark - iOSStoreKitInterface

- (void)setProvider:(id<iOSStoreKitProviderInterface>)provider {
    if (self.m_provider == provider) {
        return;
    }

    self.m_provider = provider;

    if (provider == nil) {
        [self.m_service stop];

        return;
    }

    [self.m_service start];
}

- (id<iOSStoreKitProviderInterface>)getProvider {
    return self.m_provider;
}

- (BOOL)canMakePayments {
    if (self.m_service.canMakePayments == NO) {
        return NO;
    }

    return YES;
}

- (void)requestProducts:(NSArray<NSString *> *)consumableIdentifiers nonConsumableIdentifiers:(NSArray<NSString *> *)nonConsumableIdentifiers {
    IOS_LOGGER_MESSAGE( @"request products: %@ %@"
        , consumableIdentifiers
        , nonConsumableIdentifiers
    );

    [self.m_service requestProducts:consumableIdentifiers nonConsumableIdentifiers:nonConsumableIdentifiers];
}

- (BOOL)purchaseProduct:(NSString *)productIdentifier accountToken:(NSString *)accountToken {
    IOS_LOGGER_MESSAGE( @"purchaseProduct: %@"
        , productIdentifier
    );

    if ([self.m_service purchase:productIdentifier accountToken:accountToken] == NO) {
        return NO;
    }

    return YES;
}

- (void)completeTransactionDelivery:(NSString *)transactionIdentifier successful:(BOOL)successful {
    [self.m_service completeTransactionDelivery:transactionIdentifier successful:successful];
}

- (void)restorePurchases {
    IOS_LOGGER_MESSAGE( @"restorePurchases" );

    [self.m_service restorePurchases];
}

- (BOOL)isOwnedProduct:(NSString *)productIdentifier {
    if ([[iOSStoreKitEntitlements sharedInstance] isPurchased:productIdentifier] == NO) {
        return NO;
    }

    return YES;
}

- (void)querySubscriptionStatus:(NSString *)productIdentifier {
    [self.m_service querySubscriptionStatus:productIdentifier];
}

- (BOOL)showManageSubscriptions {
    if ([self.m_service showManageSubscriptions] == NO) {
        return NO;
    }

    return YES;
}

#pragma mark - MengineStoreKitServiceDelegate

- (void)storeKitProductsResponse:(NSArray<NSDictionary *> *)products invalidIdentifiers:(NSArray<NSString *> *)invalidIdentifiers {
    IOS_LOGGER_MESSAGE( @"products response count: %lu"
        , (unsigned long)products.count
    );

    if (invalidIdentifiers.count != 0) {
        IOS_LOGGER_WARNING( @"StoreKit rejected product identifiers: %@"
            , invalidIdentifiers
        );
    }

    if (products.count == 0) {
        IOS_LOGGER_WARNING( @"StoreKit returned an empty catalog. Check Product IDs, storefront availability and Paid Apps agreement in App Store Connect." );
    }

    [self performProvider:^(id<iOSStoreKitProviderInterface> provider) {
        [provider onStoreKitProductsResponse:products];
    }];
}

- (void)storeKitProductsFailed:(NSString *)error {
    IOS_LOGGER_ERROR( @"products request failed: %@"
        , error
    );

    [self performProvider:^(id<iOSStoreKitProviderInterface> provider) {
        [provider onStoreKitProductsFailed];
    }];
}

- (void)storeKitTransaction:(NSDictionary *)transaction {
    IOS_LOGGER_MESSAGE( @"transaction: %@ product: %@"
        , transaction[@"transactionId"]
        , transaction[@"productId"]
    );

    [self performProvider:^(id<iOSStoreKitProviderInterface> provider) {
        [provider onStoreKitTransaction:transaction];
    }];
}

- (void)storeKitTransactionFinished:(NSString *)productIdentifier transactionIdentifier:(NSString *)transactionIdentifier {
    IOS_LOGGER_MESSAGE( @"finished transaction: %@ product: %@"
        , transactionIdentifier
        , productIdentifier
    );

    [self performProvider:^(id<iOSStoreKitProviderInterface> provider) {
        [provider onStoreKitTransactionFinished:productIdentifier transactionIdentifier:transactionIdentifier];
    }];
}

- (void)storeKitRestoreResult:(NSArray<NSDictionary *> *)transactions error:(NSString *)error {
    if (error != nil) {
        IOS_LOGGER_ERROR( @"restore purchases failed: %@"
            , error
        );
    } else {
        IOS_LOGGER_MESSAGE( @"restore purchases finished" );
    }

    BOOL successful = error == nil;

    [self performProvider:^(id<iOSStoreKitProviderInterface> provider) {
        [provider onStoreKitRestoreResult:transactions successful:successful];
    }];
}

- (void)storeKitPurchaseCancelled:(NSString *)productIdentifier {
    IOS_LOGGER_MESSAGE( @"purchase cancelled: %@"
        , productIdentifier
    );

    [self performProvider:^(id<iOSStoreKitProviderInterface> provider) {
        [provider onStoreKitPurchaseCancelled:productIdentifier];
    }];
}

- (void)storeKitPurchasePending:(NSString *)productIdentifier {
    IOS_LOGGER_MESSAGE( @"purchase pending: %@"
        , productIdentifier
    );

    [self performProvider:^(id<iOSStoreKitProviderInterface> provider) {
        [provider onStoreKitPurchasePending:productIdentifier];
    }];
}

- (void)storeKitPurchaseFailed:(NSString *)productIdentifier error:(NSString *)error {
    IOS_LOGGER_ERROR( @"purchase failed: %@ error: %@"
        , productIdentifier
        , error
    );

    [self performProvider:^(id<iOSStoreKitProviderInterface> provider) {
        [provider onStoreKitPurchaseFailed:productIdentifier];
    }];
}

- (void)storeKitEntitlements:(NSDictionary<NSString *, NSNumber *> *)entitlements {
    iOSStoreKitEntitlements * storage = [iOSStoreKitEntitlements sharedInstance];

    for (NSString * productIdentifier in entitlements) {
        BOOL owned = [entitlements[productIdentifier] boolValue];

        if (owned == YES) {
            [storage add:productIdentifier];
        } else {
            [storage remove:productIdentifier];
        }

        [self performProvider:^(id<iOSStoreKitProviderInterface> provider) {
            [provider onStoreKitEntitlementChanged:productIdentifier owned:owned];
        }];
    }
}

- (void)storeKitSubscriptionStatus:(NSString *)productIdentifier status:(NSDictionary *)status {
    [self performProvider:^(id<iOSStoreKitProviderInterface> provider) {
        [provider onStoreKitSubscriptionStatus:productIdentifier status:status];
    }];
}

@end
