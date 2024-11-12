#include "HttpReceiverPing.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HttpReceiverPing::HttpReceiverPing()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HttpReceiverPing::~HttpReceiverPing()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpReceiverPing::setLambda( const HttpLambdaPing & _lambda )
    {
        m_lambda = _lambda;
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpLambdaPing & HttpReceiverPing::getLambda() const
    {
        return m_lambda;
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpReceiverPing::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
    {
        bool successful = _response->isSuccessful();
        
        if( successful == false )
        {
            m_lambda( false );
            
            return;
        }
        
        EHttpCode code = _response->getCode();
        
        if( code != HTTP_OK )
        {
            m_lambda( false );
            
            return;
        }

        m_lambda( true );
    }
    //////////////////////////////////////////////////////////////////////////
}
