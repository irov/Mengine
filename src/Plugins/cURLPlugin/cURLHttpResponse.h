#pragma once

#include "Interface/HttpSystemInterface.h"

#include "curl/curl.h"

namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    class cURLHttpResponse
        : public HttpResponseInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( cURLHttpResponse );

    public:
        cURLHttpResponse();
        ~cURLHttpResponse() override;

    public:
        void setRequestId( HttpRequestId _requestId );
        void setStatus( CURLcode _status );
        void appendHeaders( const Char * _ptr, size_t _size );
        void appendData( const Char * _ptr, size_t _size );
        void setCode( EHttpCode _code );
        void setError( const String & _error );
        void setSuccessful( bool _successful );

    public:
        HttpRequestId getRequestId() const override;
        uint32_t getStatus() const override;
        const HttpRequestHeaders & getHeaders() const override;
        const String & getData() const override;
        EHttpCode getCode() const override;
        const String & getError() const override;
        bool isSuccessful() const override;

    protected:
        HttpRequestId m_requestId;
        HttpRequestHeaders m_headers;
        String m_data;
        String m_error;
        CURLcode m_status;
        EHttpCode m_code;

        bool m_successful;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHttpResponse, HttpResponseInterface> cURLHttpResponsePtr;
    //////////////////////////////////////////////////////////////////////////
}