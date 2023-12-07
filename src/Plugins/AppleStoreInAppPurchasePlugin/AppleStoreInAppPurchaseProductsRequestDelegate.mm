#import "AppleStoreInAppPurchaseProductsRequestDelegate.h"

#include "Kernel/Logger.h"

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
    
    self.m_cb->onProductResponse( self.m_request, products );
}
//////////////////////////////////////////////////////////////////////////
- (void)requestDidFinish:(SKRequest *)request {
    LOGGER_MESSAGE( "[SKProductsRequestDelegate] requestDidFinish" );
                 
    self.m_cb->onProductFinish( self.m_request );
    self.m_cb = nullptr;
    self.m_request = nullptr;
}
//////////////////////////////////////////////////////////////////////////
- (void)request:(SKRequest *)request didFailWithError:(NSError *)error {
    LOGGER_MESSAGE( "[SKProductsRequestDelegate] request didFailWithError: %s"
        , [error.localizedDescription UTF8String]
    );
    
    self.m_cb->onProductFail( self.m_request );
    self.m_cb = nullptr;
    self.m_request = nullptr;
}
//////////////////////////////////////////////////////////////////////////
@end
