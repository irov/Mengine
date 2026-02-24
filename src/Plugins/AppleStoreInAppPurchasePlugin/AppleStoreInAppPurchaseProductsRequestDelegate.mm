#import "AppleStoreInAppPurchaseProductsRequestDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSLog.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadHelper.h"

@implementation AppleStoreInAppPurchaseProductsRequestDelegate

#pragma mark -

- (instancetype _Nonnull)initWithInAppPurchase:(id<AppleStoreInAppPurchaseInterface> _Nonnull)inAppPurchase
                                       request:(const Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr &)request
                                            cb:(const Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr &)cb {
    self = [super init];
    
    self.m_inAppPurchase = inAppPurchase;
    self.m_request = request;
    self.m_cb = cb;
    
    return self;
}

-(void)dealloc {
    self.m_request = nullptr;
    self.m_cb = nullptr;
}

#pragma mark - SKProductsRequestDelegate

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
    (void)request;

    IOS_LOGGER_MESSAGE( @"[SKProductsRequestDelegate] productsRequest didReceiveResponse products: %@ invalids: %@"
        , response.products
        , response.invalidProductIdentifiers
    );
    
    NSArray<SKProduct *> * skProducts = response.products;
    
    Mengine::VectorAppleStoreInAppPurchaseProducts products;
    for( SKProduct * skProduct in skProducts ) {
        Mengine::AppleStoreInAppPurchaseProductInterfacePtr product = [self.m_inAppPurchase makeProduct:skProduct];
        
        products.emplace_back( product );
    }
    
    Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr cb_copy = self.m_cb;
    Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr request_copy = self.m_request;
    
    Mengine::Helper::dispatchMainThreadEvent([cb_copy, request_copy, products]() {
        cb_copy->onProductResponse( request_copy, products );
    });
}

- (void)requestDidFinish:(SKRequest *)request {
    (void)request;

    IOS_LOGGER_MESSAGE( @"[SKProductsRequestDelegate] requestDidFinish" );
    
    Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr cb_copy = self.m_cb;
    Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr request_copy = self.m_request;
    
    Mengine::Helper::dispatchMainThreadEvent([cb_copy, request_copy]() {
        cb_copy->onProductFinish( request_copy );
    });
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error {
    (void)request;

    IOS_LOGGER_MESSAGE( @"[SKProductsRequestDelegate] request didFailWithError: %@"
        , [AppleDetail getMessageFromNSError:error]
    );
    
    Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr cb_copy = self.m_cb;
    Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr request_copy = self.m_request;
    
    Mengine::Helper::dispatchMainThreadEvent([cb_copy, request_copy]() {
        cb_copy->onProductFail( request_copy );
    });
}

@end
