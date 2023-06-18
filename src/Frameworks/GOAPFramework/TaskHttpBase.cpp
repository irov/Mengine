#include "TaskHttpBase.h"

#include "Plugins/GOAPPlugin/GOAPInterface.h"

#include "TaskHttpReceiver.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskHttpBase::TaskHttpBase( GOAP::Allocator * _allocator, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpTaskReceiverInterfacePtr & _receiver )
        : GOAP::TaskInterface( _allocator )
        , m_url( _url )
        , m_headers( _headers )
        , m_timeout( _timeout )
        , m_receiveHeaders( _receiveHeaders )
        , m_receiver( _receiver )
        , m_requestId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskHttpBase::~TaskHttpBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskHttpBase::_onFinally()
    {
        if( m_requestId != 0 )
        {
            HTTP_SYSTEM()
                ->cancelRequest( m_requestId );

            m_requestId = 0;
        }

        m_receiver = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpReceiverInterfacePtr TaskHttpBase::createHttpReceiver_( GOAP::NodeInterface * _node )
    {
        TaskHttpReceiverPtr receiver = Helper::makeFactorableUnique<TaskHttpReceiver>( MENGINE_DOCUMENT_FUNCTION );
        
        receiver->initialize( _node, m_receiver, [this]( const HttpResponseInterfacePtr & _response )
        {
            HttpRequestId requestId = _response->getRequestId();

            if( m_requestId != requestId )
            {
                return;
            }

            m_requestId = 0;
        } );

        return receiver;
    }
    //////////////////////////////////////////////////////////////////////////
}