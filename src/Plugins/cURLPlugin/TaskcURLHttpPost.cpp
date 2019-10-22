#include "TaskcURLHttpPost.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskcURLHttpPost::TaskcURLHttpPost( const String & _url, const cURLPostParams & _params, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver )
        : TaskcURLHttpBase( _url, _timeout, _receiver )
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
            ->postMessage( m_url, m_params, m_timeout, receiver );

        if( requestId == 0 )
        {
            LOGGER_ERROR( "error http event" );

            return true;
        }

        m_requestId = requestId;

        return false;
    }
}