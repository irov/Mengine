#pragma once

#include "Plugins/cURLPlugin/cURLInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct RequestData
    {
        String type;
        HttpRequestID id;
        String url;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<RequestData> VectorRequestData;
    //////////////////////////////////////////////////////////////////////////
    class cURLRequestLogger
        : public Factorable
        , public cURLRequestLoggerInterface
    {
    public:
        cURLRequestLogger();
        ~cURLRequestLogger() override;

    public:
        void request( HttpRequestID  _id, const String & _url ) override;
        void response( HttpRequestID  _id, const String & _url ) override;

    public:
        void getPreparedData( VectorRequestData * _outData );

    protected:
        VectorRequestData m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLRequestLogger> cURLRequestLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}