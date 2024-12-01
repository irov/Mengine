#include "HttpResponse.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HttpResponse::HttpResponse()
        : m_errorCode( 0 )
        , m_code( HTTP_Unknown )
        , m_successful( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HttpResponse::~HttpResponse()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpResponse::setRequest( const HttpRequestInterfacePtr & _request )
    {
        m_request = _request;
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpRequestInterfacePtr & HttpResponse::getRequest() const
    {
        return m_request;
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpResponse::appendHeaders( const Char * _ptr, size_t _size )
    {
        m_headers.emplace_back( _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpHeaders & HttpResponse::getHeaders() const
    {
        return m_headers;
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpResponse::appendJSON( const void * _ptr, size_t _size )
    {
        const Char * json_begin = static_cast<const Char *>(_ptr);
        const Char * json_end = json_begin + _size;

        m_json.insert( m_json.end(), json_begin, json_end );
    }
    //////////////////////////////////////////////////////////////////////////
    const String & HttpResponse::getJSON() const
    {
        return m_json;
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpResponse::appendData( const void * _ptr, size_t _size )
    {
        const uint8_t * data_begin = static_cast<const uint8_t *>(_ptr);
        const uint8_t * data_end = data_begin + _size;

        m_data.insert( m_data.end(), data_begin, data_end );
    }
    //////////////////////////////////////////////////////////////////////////
    const Data & HttpResponse::getData() const
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
