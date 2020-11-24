#include "cURLRequestListener.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLRequestListener::cURLRequestListener()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLRequestListener::~cURLRequestListener()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLRequestListener::request( HttpRequestID _id, const String & _url )
    {
        RequestData requestData;

        requestData.id = _id;
        requestData.url = _url;
        requestData.type = "Request";

        m_data.push_back( requestData );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLRequestListener::response( HttpRequestID _id, const String & _url )
    {
        RequestData responseData;

        responseData.id = _id;
        responseData.url = _url;
        responseData.type = "Response";

        m_data.push_back( responseData );
    }
    //////////////////////////////////////////////////////////////////////////
    const ListRequestData & cURLRequestListener::getRequestData() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
}