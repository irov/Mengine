#import "AppleStoreInAppPurchasePaymentQueueDelegate.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"

@implementation AppleStoreInAppPurchasePaymentQueueDelegate

@synthesize m_factory;
@synthesize m_service;

#pragma mark -

- (instancetype _Nonnull)initWithFactory: (Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull)_factory service: (Mengine::AppleStoreInAppPurchaseServiceInterface * _Nonnull)_service {
    self = [super init];
    
    self.m_factory = _factory;
    self.m_service = _service;
    
    return self;
}

#pragma mark - SKPaymentQueueDelegate

// Sent when the storefront changes while a payment is processing.
- (BOOL)paymentQueue:(SKPaymentQueue *)paymentQueue shouldContinueTransaction:(SKPaymentTransaction *)transaction inStorefront:(SKStorefront *)newStorefront {
    LOGGER_MESSAGE( "SKPaymentQueueDelegate paymentQueue shouldContinueTransaction" );
    
    const Mengine::AppleStoreInAppPurchasePaymentQueueProviderInterfacePtr & provider = m_service->getPaymentQueueProvider();
    
    Mengine::AppleStoreInAppPurchasePaymentTransactionInterfacePtr paymentTransaction = m_factory->makePaymentTransaction( transaction );
    
    provider->onPaymentQueueShouldContinueTransaction( paymentTransaction );
}

// Sent if there is a pending price consent confirmation from the App Store for the current user. Return YES to immediately show the price consent UI. Return NO if you intend to show it at a later time. Defaults to YES.
// This may be called at any time that you have transaction observers on the payment queue, so make sure to set the delegate before adding any transaction observers if you intend to implement this method.
- (BOOL)paymentQueueShouldShowPriceConsent:(SKPaymentQueue *)paymentQueue {
    LOGGER_MESSAGE( "SKPaymentQueueDelegate paymentQueue paymentQueueShouldShowPriceConsent" );
    
    const Mengine::AppleStoreInAppPurchasePaymentQueueProviderInterfacePtr & provider = m_service->getPaymentQueueProvider();
    
    provider->onPaymentQueueShouldShowPriceConsent();
}

@end
