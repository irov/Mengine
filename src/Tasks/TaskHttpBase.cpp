#include "TaskHttpBase.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"

#include "GOAP/Source.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskHttpBase::TaskcURLReceiver
        : public cURLReceiverInterface
        , public Mengine::Factorable
    {
    public:
        TaskcURLReceiver( TaskHttpBase * _task, const LambdacURLReceiver & _lambda )
            : m_task( _task )
            , m_lambda( _lambda )
        {
        }

        ~TaskcURLReceiver() override
        {
        }

    protected:
        void onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _error, const String & _response, uint32_t _code, bool _successful ) override
        {
            GOAP::SourcePtr source = GOAP::Helper::makeSource();

            bool skip = m_task->isSkip();
            source->setSkip( skip );

            m_lambda( source, _status, _error, _response, _code, _successful );
            
            if( m_task->injectSource( source ) == false )
            {
                LOGGER_ERROR( "TaskHttpBase invalid inject source" );
            }
            
            m_task->requestComplete_( _id );
        }

    protected:
        TaskHttpBase * m_task;
        LambdacURLReceiver m_lambda;
    };
    //////////////////////////////////////////////////////////////////////////
    TaskHttpBase::TaskHttpBase( const String & _url, int32_t _timeout, const LambdacURLReceiver & _lambda )
        : m_url( _url )
        , m_timeout( _timeout )
        , m_lambda( _lambda )
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
            CURL_SERVICE()
                ->cancelRequest( m_requestId );

            m_requestId = 0;
        }

        m_lambda = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    cURLReceiverInterfacePtr TaskHttpBase::createHttpReceiver_()
    {
        cURLReceiverInterfacePtr receiver = Helper::makeFactorableUnique<TaskHttpBase::TaskcURLReceiver>( this, m_lambda );

        return receiver;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskHttpBase::requestComplete_( HttpRequestID _requestId )
    {
        if( m_requestId != _requestId )
        {
            return;
        }

        m_requestId = 0;

        this->complete();
    }

}