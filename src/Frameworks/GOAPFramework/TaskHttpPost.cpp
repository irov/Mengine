#include "TaskHttpPost.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskHttpPost::TaskHttpPost( GOAP::Allocator * _allocator, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpRequestPostParams & _params, const HttpTaskReceiverInterfacePtr & _receiver )
        : TaskHttpBase( _allocator, _url, _headers, _timeout, _receiveHeaders, _receiver )
        , m_params( _params )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskHttpPost::~TaskHttpPost()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskHttpPost::_onRun( GOAP::NodeInterface * _node )
    {
        HttpReceiverInterfacePtr receiver = this->createHttpReceiver_( _node );

        HttpRequestId requestId = HTTP_SYSTEM()
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