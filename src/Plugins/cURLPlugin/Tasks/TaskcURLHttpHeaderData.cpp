#include "TaskcURLHttpHeaderData.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskcURLHttpHeaderData::TaskcURLHttpHeaderData( GOAP::Allocator * _allocator, const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const cURLTaskReceiverInterfacePtr & _receiver )
        : TaskcURLHttpBase( _allocator, _url, _headers, _timeout, _receiveHeaders, _receiver )
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
            ->headerData( m_url, m_headers, m_timeout, m_receiveHeaders, m_data, receiver, MENGINE_DOCUMENT_FUNCTION );

        if( requestId == 0 )
        {
            LOGGER_ERROR( "error http event" );

            return true;
        }

        m_requestId = requestId;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}