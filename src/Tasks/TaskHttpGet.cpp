#include "TaskHttpGet.h"

#include "Kernel/Logger.h"

namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    TaskHttpGet::TaskHttpGet( const String & _url, int32_t _timeout, const LambdacURLReceiver & _lambda )
        : TaskHttpBase( _url, _timeout, _lambda )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskHttpGet::~TaskHttpGet()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskHttpGet::_onRun()
    {
        cURLReceiverInterfacePtr receiver = this->createHttpReceiver_();

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