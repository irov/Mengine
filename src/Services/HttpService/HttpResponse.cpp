#include "HttpResponse.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HttpResponse::HttpResponse()
        : m_requestId( INVALID_UNIQUE_ID )
        , m_errorCode( 0 )
        , m_code( HTTP_Unknown )
        , m_successful( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HttpResponse::~HttpResponse()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpResponse::setRequestId( HttpRequestId _requestId )
    {
        m_requestId = _requestId;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId HttpResponse::getRequestId() const
    {
        return m_requestId;
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpResponse::appendHeaders( const Char * _ptr, size_t _size )
    {
        m_headers.emplace_back( _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpRequestHeaders & HttpResponse::getHeaders() const
    {
        return m_headers;
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpResponse::appendData( const Char * _ptr, size_t _size )
    {
        m_data.append( _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    const String & HttpResponse::getData() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpResponse::setCode( EHttpCode _code )
    {
        m_code = _code;
    }
    //////////////////////////////////////////////////////////////////////////
    EHttpCode HttpResponse::getCode() const
    {
        return m_code;
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpResponse::setError( const String & _message, int32_t _code )
    {
        m_errorMessage = _message;
        m_errorCode = _code;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & HttpResponse::getErrorMessage() const
    {
        return m_errorMessage;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t HttpResponse::getErrorCode() const
    {
        return m_errorCode;
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpResponse::setSuccessful( bool _successful )
    {
        m_successful = _successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HttpResponse::isSuccessful() const
    {
        return m_successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
