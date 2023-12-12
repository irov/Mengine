#import "AppleStoreInAppPurchaseProductsRequestDelegate.h"

#include "Kernel/Logger.h"
#include "Kernel/PlatformHelper.h"

//////////////////////////////////////////////////////////////////////////
@implementation AppleStoreInAppPurchaseProductsRequestDelegate
//////////////////////////////////////////////////////////////////////////
#pragma mark -
//////////////////////////////////////////////////////////////////////////
- (instancetype _Nonnull)initWithFactory:(Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull)_factory request:(const Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr &)_request cb: (const Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr &)_cb {
    self = [super init];
    
    self.m_factory = _factory;
    self.m_request = _request;
    self.m_cb = _cb;
    
    return self;
}
//////////////////////////////////////////////////////////////////////////
-(void)dealloc {
    self.m_request = nullptr;
    self.m_cb = nullptr;
}
//////////////////////////////////////////////////////////////////////////
#pragma mark - SKProductsRequestDelegate
//////////////////////////////////////////////////////////////////////////
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
    LOGGER_MESSAGE( "[SKProductsRequestDelegate] productsRequest didReceiveResponse:" );
    
    NSArray<SKProduct *> * skProducts = response.products;
    
    Mengine::VectorAppleStoreInAppPurchaseProducts products;
    for( SKProduct * skProduct in skProducts )
    {
        LOGGER_MESSAGE( "skProduct: %s"
            , [skProduct.productIdentifier UTF8String]
        );
        
        Mengine::AppleStoreInAppPurchaseProductInterfacePtr product = self.m_factory->makeProduct( skProduct );
        
        products.emplace_back( product );
    }
    
    Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr cb_copy = self.m_cb;
    Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr request_copy = self.m_request;
    
    Mengine::Helper::dispatchMainThreadEvent([cb_copy, request_copy, products]() {
        cb_copy->onProductResponse( request_copy, products );
    });
}
//////////////////////////////////////////////////////////////////////////
- (void)requestDidFinish:(SKRequest *)request {
    LOGGER_MESSAGE( "[SKProductsRequestDelegate] requestDidFinish" );
    
    Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr cb_copy = self.m_cb;
    Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr request_copy = self.m_request;
    
    Mengine::Helper::dispatchMainThreadEvent([cb_copy, request_copy]() {
        cb_copy->onProductFinish( request_copy );
    });
    
    self.m_cb = nullptr;
    self.m_request = nullptr;
}
//////////////////////////////////////////////////////////////////////////
- (void)request:(SKRequest *)request didFailWithError:(NSError *)error {
    LOGGER_MESSAGE( "[SKProductsRequestDelegate] request didFailWithError: %s"
        , [error.localizedDescription UTF8String]
    );
    
    Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr cb_copy = self.m_cb;
    Mengine::AppleStoreInAppPurchaseProductsRequestInterfacePtr request_copy = self.m_request;
    
    Mengine::Helper::dispatchMainThreadEvent([cb_copy, request_copy]() {
        cb_copy->onProductFail( request_copy );
    });
    
    self.m_cb = nullptr;
    self.m_request = nullptr;
}
//////////////////////////////////////////////////////////////////////////
@end
