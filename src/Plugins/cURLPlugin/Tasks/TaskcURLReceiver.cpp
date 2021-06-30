#include "TaskcURLReceiver.h"

#include "Plugins/GOAPPlugin/GOAPInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskcURLReceiver::TaskcURLReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskcURLReceiver::~TaskcURLReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskcURLReceiver::initialize( GOAP::NodeInterface * _node, const cURLTaskReceiverInterfacePtr & _receiver, const LambdaRequestId & _lambda )
    {
        m_node = _node;
        m_receiver = _receiver;
        m_lambda = _lambda;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskcURLReceiver::finalize()
    {
        m_node = nullptr;
        m_receiver = nullptr;
        m_lambda = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskcURLReceiver::onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _error, const cURLHeaders & _headers, const String & _response, uint32_t _code, bool _successful )
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

        m_lambda( _id );

        m_node->complete();
    }
    //////////////////////////////////////////////////////////////////////////
}