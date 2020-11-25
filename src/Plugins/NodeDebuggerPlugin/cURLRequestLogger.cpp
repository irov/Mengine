#include "cURLRequestLogger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLRequestLogger::cURLRequestLogger()
        : m_currentRequestId( 0 )
        , m_lastSendRequestsId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLRequestLogger::~cURLRequestLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLRequestLogger::request( HttpRequestID _id, const String & _url )
    {
        RequestData requestData;

        requestData.id = _id;
        requestData.url = _url;
        requestData.type = "Request";

        m_data.push_back( requestData );

        m_currentRequestId = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLRequestLogger::response( HttpRequestID _id, const String & _url )
    {
        RequestData responseData;

        responseData.id = _id;
        responseData.url = _url;
        responseData.type = "Response";

        m_data.push_back( responseData );

        m_currentRequestId = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLRequestLogger::getPreparedData( VectorRequestData * _outData )
    {
        uint32_t dataSize = m_data.size();
        if( dataSize <= m_lastSendRequestsId )
        {
            return;
        }

        std::copy( m_data.begin() + m_lastSendRequestsId, m_data.end(), std::back_inserter( *_outData ) );

        m_lastSendRequestsId = dataSize - 1;
    }
    //////////////////////////////////////////////////////////////////////////
}