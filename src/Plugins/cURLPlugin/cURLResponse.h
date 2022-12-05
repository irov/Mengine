#pragma once

#include "cURLInterface.h"

#include "curl/curl.h"

namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    class cURLResponse
        : public cURLResponseInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( cURLResponse );

    public:
        cURLResponse();
        ~cURLResponse() override;

    public:
        void setRequestId( HttpRequestID _requestId );
        void setStatus( CURLcode _status );
        void appendHeaders( const Char * _ptr, size_t _size );
        void appendData( const Char * _ptr, size_t _size );
        void setCode( uint32_t _code );
        void setError( const String & _error );
        void setSuccessful( bool _successful );

    public:
        HttpRequestID getRequestId() const override;
        uint32_t getStatus() const override;
        const cURLHeaders & getHeaders() const override;
        const String & getData() const override;
        uint32_t getCode() const override;
        const String & getError() const override;
        bool isSuccessful() const override;

    protected:
        HttpRequestID m_requestId;
        cURLHeaders m_headers;
        String m_data;
        String m_error;
        CURLcode m_status;
        uint32_t m_code;

        bool m_successful;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLResponse, cURLResponseInterface> cURLResponsePtr;
    //////////////////////////////////////////////////////////////////////////
}