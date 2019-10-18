#include "TaskcURLHttpHeaderData.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskcURLHttpHeaderData::TaskcURLHttpHeaderData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver )
        : TaskcURLHttpBase( _url, _timeout, _receiver )
        , m_headers( _headers )
        , m_data( _data )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskcURLHttpHeaderData::~TaskcURLHttpHeaderData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskcURLHttpHeaderData::_onRun( GOAP::NodeInterface * _node )
    {
        cURLReceiverInterfacePtr receiver = this->createHttpReceiver_( _node );

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