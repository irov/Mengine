#pragma once

#include "Interface/HttpResponseInterface.h"

namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    class HttpResponse
        : public HttpResponseInterface
    {
        DECLARE_FACTORABLE( HttpResponse );

    public:
        HttpResponse();
        ~HttpResponse() override;

    public:
        void setRequestId( HttpRequestId _requestId ) override;
        HttpRequestId getRequestId() const override;

        void appendHeaders( const Char * _ptr, size_t _size ) override;
        const HttpRequestHeaders & getHeaders() const override;

        void appendData( const Char * _ptr, size_t _size ) override;
        const String & getData() const override;

        void setCode( EHttpCode _code ) override;
        EHttpCode getCode() const override;

        void setError( const String & _message, int32_t _code ) override;
        const String & getErrorMessage() const override;
        int32_t getErrorCode() const override;

        void setSuccessful( bool _successful ) override;
        bool isSuccessful() const override;

    protected:
        HttpRequestId m_requestId;
        HttpRequestHeaders m_headers;
        String m_data;
        String m_errorMessage;
        int32_t m_errorCode;
        EHttpCode m_code;

        bool m_successful;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HttpResponse, HttpResponseInterface> AndroidHttpResponsePtr;
    //////////////////////////////////////////////////////////////////////////
}
