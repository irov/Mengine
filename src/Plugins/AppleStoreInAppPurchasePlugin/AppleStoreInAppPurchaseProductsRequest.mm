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
        m_skProductsRequest = nil;
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseProductsRequest::setSKProductsRequest( SKProductsRequest * _skProductsRequest )
    {
        m_skProductsRequest = _skProductsRequest;
    }
    /////////////////////////////////////////////////////////////////////////////
    SKProductsRequest * AppleStoreInAppPurchaseProductsRequest::getSKProductsRequest() const
    {
        return m_skProductsRequest;
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseProductsRequest::cancel()
    {
        LOGGER_MESSAGE( "products request cancel" );
        
        [m_skProductsRequest cancel];
    }
    /////////////////////////////////////////////////////////////////////////////
}
