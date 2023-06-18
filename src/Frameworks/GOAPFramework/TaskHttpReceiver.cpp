#include "TaskHttpReceiver.h"

#include "Plugins/GOAPPlugin/GOAPInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskHttpReceiver::TaskHttpReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskHttpReceiver::~TaskHttpReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskHttpReceiver::initialize( GOAP::NodeInterface * _node, const HttpTaskReceiverInterfacePtr & _receiver, const LambdaRequestId & _lambda )
    {
        m_node = _node;
        m_receiver = _receiver;
        m_lambda = _lambda;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskHttpReceiver::finalize()
    {
        m_node = nullptr;
        m_receiver = nullptr;
        m_lambda = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskHttpReceiver::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
    {
        GOAP::SourceInterfacePtr source = GOAP_SERVICE()
            ->makeSource();

        m_receiver->onResponse( source, _response );
        m_receiver = nullptr;

        const GOAP::SourceProviderInterfacePtr & provider = source->getSourceProvider();

        bool skip = m_node->isSkip();
        provider->setSkip( skip );

        if( m_node->injectSource( provider ) == false )
        {
            LOGGER_ERROR( "TaskHttpBase invalid inject source" );
        }

        m_lambda( _response );

        m_node->complete();
    }
    //////////////////////////////////////////////////////////////////////////
}