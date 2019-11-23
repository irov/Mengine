#include "TaskcURLHttpPost.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskcURLHttpPost::TaskcURLHttpPost( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _cookies, const cURLPostParams & _params, const cURLTaskReceiverInterfacePtr & _receiver )
        : TaskcURLHttpBase( _url, _headers, _timeout, _receiveHeaders, _receiver )
        , m_cookies( _cookies )
        , m_params( _params )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskcURLHttpPost::~TaskcURLHttpPost()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskcURLHttpPost::_onRun( GOAP::NodeInterface * _node )
    {
        cURLReceiverInterfacePtr receiver = this->createHttpReceiver_( _node );

        HttpRequestID requestId = CURL_SERVICE()
            ->postMessage( m_url, m_headers, m_timeout, m_receiveHeaders, m_cookies, m_params, receiver );

        if( requestId == 0 )
        {
            LOGGER_ERROR( "error http event" );

            return true;
        }

        m_requestId = requestId;

        return false;
    }
}