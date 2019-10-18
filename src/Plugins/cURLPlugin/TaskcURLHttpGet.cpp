#include "TaskcURLHttpGet.h"

#include "Kernel/Logger.h"

namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    TaskcURLHttpGet::TaskcURLHttpGet( const String & _url, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver )
        : TaskcURLHttpBase( _url, _timeout, _receiver )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskcURLHttpGet::~TaskcURLHttpGet()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskcURLHttpGet::_onRun( GOAP::NodeInterface * _node )
    {
        cURLReceiverInterfacePtr receiver = this->createHttpReceiver_( _node );

        HttpRequestID requestId = CURL_SERVICE()
            ->getMessage( m_url, m_timeout, receiver );

        if( requestId == 0 )
        {
            LOGGER_ERROR( "error http event" );

            return true;
        }

        m_requestId = requestId;

        return false;
    }
}