#import "AppleStoreInAppPurchaseProductsRequestDelegate.h"

#import "Environment/Apple/AppleDetail.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadHelper.h"

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
    NSMutableString * skProductsLog = [NSMutableString stringWithString:@""];
    
    NSArray<SKProduct *> * skProducts = response.products;
    
    Mengine::VectorAppleStoreInAppPurchaseProducts products;
    for( SKProduct * skProduct in skProducts )
    {
        [skProductsLog appendString:skProduct.productIdentifier];
        [skProductsLog appendString:@", "];
        
        Mengine::AppleStoreInAppPurchaseProductInterfacePtr product = self.m_factory->makeProduct( skProduct );
        
        products.emplace_back( product );
    }
    
    LOGGER_MESSAGE( "[SKProductsRequestDelegate] productsRequest didReceiveResponse: %s"
        , [skProductsLog UTF8String]
    );
    
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
        , [[AppleDetail getMessageFromNSError:error] UTF8String]
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
