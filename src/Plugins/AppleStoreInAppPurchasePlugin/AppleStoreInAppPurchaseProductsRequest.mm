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
        m_skProductsDelegate = nil;
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseProductsRequest::setSKProductsRequest( SKProductsRequest * _skProductsRequest, id<SKProductsRequestDelegate> _skProductsDelegate )
    {
        m_skProductsRequest = _skProductsRequest;
        m_skProductsDelegate = _skProductsDelegate;
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
