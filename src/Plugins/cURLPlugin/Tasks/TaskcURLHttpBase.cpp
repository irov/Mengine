#include "TaskcURLHttpBase.h"

#include "Plugins/GOAPPlugin/GOAPInterface.h"

#include "TaskcURLReceiver.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskcURLHttpBase::TaskcURLHttpBase( GOAP::Allocator * _allocator, const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLTaskReceiverInterfacePtr & _receiver )
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
    TaskcURLHttpBase::~TaskcURLHttpBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskcURLHttpBase::_onFinally()
    {
        if( m_requestId != 0 )
        {
            CURL_SERVICE()
                ->cancelRequest( m_requestId );

            m_requestId = 0;
        }

        m_receiver = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    cURLReceiverInterfacePtr TaskcURLHttpBase::createHttpReceiver_( GOAP::NodeInterface * _node )
    {
        TaskcURLReceiverPtr receiver = Helper::makeFactorableUnique<TaskcURLReceiver>( MENGINE_DOCUMENT_FUNCTION );
        
        receiver->initialize( _node, m_receiver, [this]( HttpRequestID _requestId )
        {
            if( m_requestId != _requestId )
            {
                return;
            }

            m_requestId = 0;
        } );

        return receiver;
    }
    //////////////////////////////////////////////////////////////////////////
}