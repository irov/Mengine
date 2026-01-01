#import "AppleStoreInAppPurchaseApplicationDelegate.h"

#import "AppleStoreInAppPurchasePaymentTransactionObserver.h"

#import "Environment/iOS/iOSLog.h"

@implementation AppleStoreInAppPurchaseApplicationDelegate

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // According to Apple documentation, the SKPaymentQueueDelegate must be set BEFORE adding transaction observers
    // This is critical for proper IAP functionality on iPad devices
    // The delegate implements paymentQueueShouldShowPriceConsent which is required for IAP to work correctly
    [[SKPaymentQueue defaultQueue] setDelegate:self];
    
    AppleStoreInAppPurchasePaymentTransactionObserver * paymentTransactionObserver = [AppleStoreInAppPurchasePaymentTransactionObserver sharedInstance];
    
    [[SKPaymentQueue defaultQueue] addTransactionObserver:paymentTransactionObserver];
    
    return YES;
}

- (void)applicationWillTerminate:(UIApplication *)application {
    AppleStoreInAppPurchasePaymentTransactionObserver * paymentTransactionObserver = [AppleStoreInAppPurchasePaymentTransactionObserver sharedInstance];
    
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:paymentTransactionObserver];
}

#pragma mark - SKPaymentQueueDelegate

// Sent when the storefront changes while a payment is processing.
- (BOOL)paymentQueue:(SKPaymentQueue *)paymentQueue shouldContinueTransaction:(SKPaymentTransaction *)transaction inStorefront:(SKStorefront *)newStorefront {
    IOS_LOGGER_MESSAGE( @"SKPaymentQueueDelegate paymentQueue: %@ shouldContinueTransaction: %@ inStorefront: %@"
        , paymentQueue
        , transaction
        , newStorefront
    );
    
    return YES;
}

// Sent if there is a pending price consent confirmation from the App Store for the current user. Return YES to immediately show the price consent UI. Return NO if you intend to show it at a later time. Defaults to YES.
// This may be called at any time that you have transaction observers on the payment queue, so make sure to set the delegate before adding any transaction observers if you intend to implement this method.
- (BOOL)paymentQueueShouldShowPriceConsent:(SKPaymentQueue *)paymentQueue {
    IOS_LOGGER_MESSAGE( @"SKPaymentQueueDelegate paymentQueueShouldShowPriceConsent: %@"
        , paymentQueue
    );
    
    return YES;
}

@end
