#import "AppleStoreInAppPurchaseProductsRequestDelegate.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
@implementation AppleStoreInAppPurchaseProductsRequestDelegate
//////////////////////////////////////////////////////////////////////////
@synthesize m_factory;
@synthesize m_cb;
//////////////////////////////////////////////////////////////////////////
#pragma mark -
//////////////////////////////////////////////////////////////////////////
- (instancetype _Nonnull)initWithFactory: (Mengine::AppleStoreInAppPurchaseFactoryInterface * _Nonnull)_factory cb: (Mengine::AppleStoreInAppPurchaseProductsResponseInterfacePtr)_cb {
    self = [super init];
    
    self.m_factory = _factory;
    self.m_cb = _cb;
        
    return self;
}
//////////////////////////////////////////////////////////////////////////
#pragma mark - SKProductsRequestDelegate
//////////////////////////////////////////////////////////////////////////
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
    LOGGER_INFO( "inapppurchase", "productsRequest didReceiveResponse:" );
    
    NSArray<SKProduct *> * skProducts = response.products;
    
    Mengine::VectorAppleStoreInAppPurchaseProducts products;
    for( SKProduct * skProduct in skProducts )
    {
        LOGGER_INFO( "inapppurchase", "skproduct: %s"
            , [skProduct.productIdentifier UTF8String]
        );
        
        Mengine::AppleStoreInAppPurchaseProductInterfacePtr product = m_factory->makeProduct( skProduct );
        
        products.emplace_back( product );
    }
    
    m_cb->onProductResponse( products );
}
//////////////////////////////////////////////////////////////////////////
- (void)requestDidFinish:(SKRequest *)request {
    LOGGER_INFO( "inapppurchase", "requestDidFinish" );
                 
    m_cb->onProductFinish();
    m_cb = nullptr;
}
//////////////////////////////////////////////////////////////////////////
- (void)request:(SKRequest *)request didFailWithError:(NSError *)error {
    LOGGER_ERROR( "request didFailWithError: %s"
        , [error.localizedDescription UTF8String]
    );
    
    m_cb->onProductFail();
}
//////////////////////////////////////////////////////////////////////////
@end
