#include "TaskHttpHeaderData.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskHttpHeaderData::TaskHttpHeaderData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const LambdacURLReceiver & _lambda )
        : TaskHttpBase( _url, _timeout, _lambda )
        , m_headers( _headers )
        , m_data( _data )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskHttpHeaderData::~TaskHttpHeaderData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskHttpHeaderData::_onRun()
    {
        cURLReceiverInterfacePtr receiver = this->createHttpReceiver_();

        HttpRequestID requestId = CURL_SERVICE()
            ->headerData( m_url, m_headers, m_data, m_timeout, receiver );

        if( requestId == 0 )
        {
            LOGGER_ERROR( "error http event" );

            return true;
        }

        m_requestId = requestId;

        return false;
    }
}