#include "TaskcURLHttpBase.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

#include "Plugins/GOAPPlugin/GOAPInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskcURLHttpBase::TaskcURLReceiver
        : public cURLReceiverInterface
        , public Mengine::Factorable
    {
    public:
        TaskcURLReceiver( GOAP::NodeInterface * _node, TaskcURLHttpBase * _task, const cURLTaskReceiverInterfacePtr & _receiver )
            : m_node( _node )
            , m_task( _task )
            , m_receiver( _receiver )
        {
        }

        ~TaskcURLReceiver() override
        {
        }

    protected:
        void onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _error, const cURLHeaders & _headers, const String & _response, uint32_t _code, bool _successful ) override
        {
            GOAP::SourceInterfacePtr source = GOAP_SERVICE()
                ->makeSource();

            m_receiver->onResponse( source, _status, _error, _headers, _response, _code, _successful );
            m_receiver = nullptr;

            const GOAP::SourceProviderInterfacePtr & provider = source->getSourceProvider();

            bool skip = m_node->isSkip();
            provider->setSkip( skip );
            
            if( m_node->injectSource( provider ) == false )
            {
                LOGGER_ERROR( "TaskHttpBase invalid inject source" );
            }
            
            m_task->requestComplete_( _id );
            
            m_node->complete();
        }

    protected:
        GOAP::NodeInterfacePtr m_node;
        TaskcURLHttpBasePtr m_task;
        cURLTaskReceiverInterfacePtr m_receiver;
    };
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
        cURLReceiverInterfacePtr receiver = Helper::makeFactorableUnique<TaskcURLHttpBase::TaskcURLReceiver>( MENGINE_DOCUMENT_FUNCTION, _node, this, m_receiver );

        return receiver;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskcURLHttpBase::requestComplete_( HttpRequestID _requestId )
    {
        if( m_requestId != _requestId )
        {
            return;
        }

        m_requestId = 0;
    }
    //////////////////////////////////////////////////////////////////////////
}