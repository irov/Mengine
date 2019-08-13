#include "TaskHttpPost.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskHttpPost::TaskHttpPost( const String & _url, const MapParams & _params, int32_t _timeout, const LambdacURLReceiver & _lambda )
        : TaskHttpBase( _url, _timeout, _lambda )
        , m_params( _params )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskHttpPost::~TaskHttpPost()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskHttpPost::_onRun()
    {
        cURLReceiverInterfacePtr receiver = this->createHttpReceiver_();

        HttpRequestID requestId = CURL_SERVICE()
            ->postMessage( m_url, m_params, m_timeout, receiver );

        if( requestId == 0 )
        {
            LOGGER_ERROR( "error http event" );

            return true;
        }

        m_requestId = requestId;

        return false;
    }
}