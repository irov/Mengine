#include "TaskHttpHeaderData.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskHttpHeaderData::TaskHttpHeaderData( GOAP::Allocator * _allocator, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const HttpTaskReceiverInterfacePtr & _receiver )
        : TaskHttpBase( _allocator, _url, _headers, _timeout, _receiveHeaders, _receiver )
        , m_data( _data )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskHttpHeaderData::~TaskHttpHeaderData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskHttpHeaderData::_onRun( GOAP::NodeInterface * _node )
    {
        HttpReceiverInterfacePtr receiver = this->createHttpReceiver_( _node );

        HttpRequestId requestId = HTTP_SYSTEM()
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