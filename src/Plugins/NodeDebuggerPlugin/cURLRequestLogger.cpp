#include "cURLRequestLogger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLRequestLogger::cURLRequestLogger()
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
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLRequestLogger::response( HttpRequestID _id, const String & _url )
    {
        RequestData responseData;

        responseData.id = _id;
        responseData.url = _url;
        responseData.type = "Response";

        m_data.push_back( responseData );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLRequestLogger::getPreparedData( VectorRequestData * _outData )
    {
        std::copy( m_data.begin(), m_data.end(), std::back_inserter( *_outData ) );
    }
    //////////////////////////////////////////////////////////////////////////
}