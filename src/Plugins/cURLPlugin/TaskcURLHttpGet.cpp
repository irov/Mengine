#include "TaskcURLHttpGet.h"

#include "Kernel/Logger.h"

namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    TaskcURLHttpGet::TaskcURLHttpGet( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLTaskReceiverInterfacePtr & _receiver )
        : TaskcURLHttpBase( _url, _headers, _timeout, _receiveHeaders, _receiver )
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
            ->getMessage( m_url, m_headers, m_timeout, m_receiveHeaders, receiver );

        if( requestId == 0 )
        {
            LOGGER_ERROR( "error http event" );

            return true;
        }

        m_requestId = requestId;

        return false;
    }
}