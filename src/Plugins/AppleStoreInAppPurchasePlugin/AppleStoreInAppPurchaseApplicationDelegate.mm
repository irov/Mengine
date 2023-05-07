#import "AppleStoreInAppPurchaseApplicationDelegate.h"

#import "AppleStoreInAppPurchasePaymentTransactionObserver.h"

@implementation AppleStoreInAppPurchaseApplicationDelegate

#pragma mark - UIKitProxyApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    AppleStoreInAppPurchasePaymentTransactionObserver * paymentTransactionObserver = [AppleStoreInAppPurchasePaymentTransactionObserver sharedInstance];
    
    [[SKPaymentQueue defaultQueue] addTransactionObserver:paymentTransactionObserver];
    
    return YES;
}

- (void)applicationWillTerminate:(UIApplication *)application {
    AppleStoreInAppPurchasePaymentTransactionObserver * paymentTransactionObserver = [AppleStoreInAppPurchasePaymentTransactionObserver sharedInstance];
    
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:paymentTransactionObserver];
}

@end
