#include "iOSStoreInAppPurchaseProductsRequest.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSStoreInAppPurchaseProductsRequest::iOSStoreInAppPurchaseProductsRequest()
        : m_skProductsRequest( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSStoreInAppPurchaseProductsRequest::~iOSStoreInAppPurchaseProductsRequest()
    {
        m_skProductsRequest = nil;
        m_skProductsDelegate = nil;
    }
    /////////////////////////////////////////////////////////////////////////////
    void iOSStoreInAppPurchaseProductsRequest::setSKProductsRequest( SKProductsRequest * _skProductsRequest, id<SKProductsRequestDelegate> _skProductsDelegate )
    {
        m_skProductsRequest = _skProductsRequest;
        m_skProductsDelegate = _skProductsDelegate;
    }
    /////////////////////////////////////////////////////////////////////////////
    SKProductsRequest * iOSStoreInAppPurchaseProductsRequest::getSKProductsRequest() const
    {
        return m_skProductsRequest;
    }
    /////////////////////////////////////////////////////////////////////////////
    void iOSStoreInAppPurchaseProductsRequest::cancel()
    {
        LOGGER_MESSAGE( "products request cancel" );

        [m_skProductsRequest cancel];
    }
    /////////////////////////////////////////////////////////////////////////////
}
