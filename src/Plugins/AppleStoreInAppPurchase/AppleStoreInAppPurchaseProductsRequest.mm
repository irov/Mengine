#include "AppleStoreInAppPurchaseProductsRequest.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchaseProductsRequest::AppleStoreInAppPurchaseProductsRequest()
        : m_skProductsRequest( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchaseProductsRequest::~AppleStoreInAppPurchaseProductsRequest()
    {
        [m_skProductsRequest release];
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseProductsRequest::setSKProductsRequest( SKProductsRequest * _skProductsRequest )
    {
        m_skProductsRequest = [_skProductsRequest retain];
    }
    /////////////////////////////////////////////////////////////////////////////
    SKProductsRequest * AppleStoreInAppPurchaseProductsRequest::getSKProductsRequest() const
    {
        return m_skProductsRequest;
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseProductsRequest::cancel()
    {
        LOGGER_INFO( "inapppurchase", "products request cancel" );
        
        [m_skProductsRequest cancel];
    }
    /////////////////////////////////////////////////////////////////////////////
}
