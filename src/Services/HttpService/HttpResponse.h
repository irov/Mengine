#pragma once

#include "Interface/HttpResponseInterface.h"
#include "Interface/HttpRequestInterface.h"

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
        void setRequest( const HttpRequestInterfacePtr & _request ) override;
        const HttpRequestInterfacePtr & getRequest() const override;

        void appendHeaders( const Char * _ptr, size_t _size ) override;
        const HttpHeaders & getHeaders() const override;

        void appendJSON( const void * _ptr, size_t _size ) override;
        const String & getJSON() const override;

        void appendData( const void * _ptr, size_t _size ) override;
        const Data & getData() const override;

        void setCode( EHttpCode _code ) override;
        EHttpCode getCode() const override;

        void setError( const String & _message, int32_t _code ) override;
        const String & getErrorMessage() const override;
        int32_t getErrorCode() const override;

        void setSuccessful( bool _successful ) override;
        bool isSuccessful() const override;

    protected:
        HttpRequestInterfacePtr m_request;
        HttpHeaders m_headers;
        String m_json;
        Data m_data;
        String m_errorMessage;
        int32_t m_errorCode;
        EHttpCode m_code;

        bool m_successful;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HttpResponse, HttpResponseInterface> AndroidHttpResponsePtr;
    //////////////////////////////////////////////////////////////////////////
}
