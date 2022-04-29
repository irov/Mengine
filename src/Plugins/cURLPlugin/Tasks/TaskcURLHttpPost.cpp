#include "TaskcURLHttpPost.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskcURLHttpPost::TaskcURLHttpPost( GOAP::Allocator * _allocator, const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLPostParams & _params, const cURLTaskReceiverInterfacePtr & _receiver )
        : TaskcURLHttpBase( _allocator, _url, _headers, _timeout, _receiveHeaders, _receiver )
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
            ->postMessage( m_url, m_headers, m_timeout, m_receiveHeaders, m_params, receiver, MENGINE_DOCUMENT_FUNCTION );

        if( requestId == 0 )
        {
            LOGGER_ERROR( "error http event" );

            return true;
        }

        m_requestId = requestId;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}