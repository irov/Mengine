#import "AppleStoreInAppPurchasePaymentQueueDelegate.h"

#import "Environment/iOS/iOSLog.h"

@implementation AppleStoreInAppPurchasePaymentQueueDelegate

#pragma mark - SKPaymentQueueDelegate

// Sent when the storefront changes while a payment is processing.
- (BOOL)paymentQueue:(SKPaymentQueue *)paymentQueue shouldContinueTransaction:(SKPaymentTransaction *)transaction inStorefront:(SKStorefront *)newStorefront API_AVAILABLE(ios(13.0), macos(10.15), watchos(6.2)) {
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
