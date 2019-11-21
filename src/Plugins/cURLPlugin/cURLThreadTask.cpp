#include "cURLThreadTask.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "cURLErrorHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static int cURL_trace( CURL * handle, curl_infotype type,
            char * data, size_t size,
            void * userp )
        {            
            MENGINE_UNUSED( handle );
            MENGINE_UNUSED( data );
            MENGINE_UNUSED( size );
            MENGINE_UNUSED( userp );

            const Char * text;

            switch( type )
            {
            case CURLINFO_TEXT:
                LOGGER_MESSAGE_WN( "== Info: %s", data );
            case CURLINFO_HEADER_OUT:
                text = "=> Send header";
                break;
            case CURLINFO_DATA_OUT:
                text = "=> Send data";
                break;
            case CURLINFO_SSL_DATA_OUT:
                text = "=> Send SSL data";
                break;
            case CURLINFO_HEADER_IN:
                text = "<= Recv header";
                break;
            case CURLINFO_DATA_IN:
                text = "<= Recv data";
                break;
            case CURLINFO_SSL_DATA_IN:
                text = "<= Recv SSL data";
                break;
            default:
                return 0;
            }

            LOGGER_MESSAGE( "%s, %10.10lu bytes (0x%8.8lx)",
                text, (uint64_t)size, (uint64_t)size );

            LOGGER_MESSAGE( "%.*s", size, data );

            return 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    cURLThreadTask::cURLThreadTask()
        : m_id( 0 )
        , m_timeout( -1 )
        , m_receiveHeaders( false )
        , m_responseCode( 0 )
        , m_responseStatus( CURLE_OK )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLThreadTask::~cURLThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::setURL( const String & _url )
    {
        m_url = _url;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & cURLThreadTask::getURL() const
    {
        return m_url;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::setHeaders( const cURLHeaders & _headers )
    {
        m_headers = _headers;
    }
    //////////////////////////////////////////////////////////////////////////
    const cURLHeaders & cURLThreadTask::getHeaders() const
    {
        return m_headers;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::setRequestId( HttpRequestID _id )
    {
        m_id = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLThreadTask::getRequestId() const
    {
        return m_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::setTimeout( int32_t _timeout )
    {
        m_timeout = _timeout;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t cURLThreadTask::getTimeout() const
    {
        return m_timeout;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::setReceiveHeaders( bool _receiveHeaders )
    {
        m_receiveHeaders = _receiveHeaders;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLThreadTask::getReceiveHeaders() const
    {
        return m_receiveHeaders;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::setReceiver( const cURLReceiverInterfacePtr & _receiver )
    {
        m_receiver = _receiver;
    }
    //////////////////////////////////////////////////////////////////////////
    const cURLReceiverInterfacePtr & cURLThreadTask::getReceiver() const
    {
        return m_receiver;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLThreadTask::_onRun()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLThreadTask::_onMain()
    {
        /* init the curl session */
        CURL * curl = curl_easy_init();

        this->_onCURL( curl );

        if( m_timeout != -1 )
        {
            CURLCALL( curl_easy_setopt, (curl, CURLOPT_TIMEOUT_MS, m_timeout) );
        }

        CURLCALL( curl_easy_setopt, (curl, CURLOPT_SSL_VERIFYPEER, 0) );
        CURLCALL( curl_easy_setopt, (curl, CURLOPT_SSL_VERIFYHOST, 0) );


        char errorbuf[CURL_ERROR_SIZE] = { '\0' };
        CURLCALL( curl_easy_setopt, (curl, CURLOPT_ERRORBUFFER, errorbuf) );

        if( CONFIG_VALUE( "HTTP", "Verbose", false ) == true )
        {
            CURLCALL( curl_easy_setopt, (curl, CURLOPT_VERBOSE, 1) );
        }

        if( CONFIG_VALUE( "HTTP", "Trace", false ) == true )
        {            
            CURLCALL( curl_easy_setopt, (curl, CURLOPT_DEBUGFUNCTION, &Detail::cURL_trace) );
            CURLCALL( curl_easy_setopt, (curl, CURLOPT_DEBUGDATA, nullptr) );
        }

        CURLcode status = curl_easy_perform( curl );

        m_responseStatus = status;
        m_responseError = errorbuf;

        long http_code = 0;
        CURLCALL( curl_easy_getinfo, (curl, CURLINFO_RESPONSE_CODE, &http_code) );

        m_responseCode = (uint32_t)http_code;

        this->_onCURLCleanup( curl );

        curl_easy_cleanup( curl );

        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::_onCURLCleanup( CURL * _curl )
    {
        MENGINE_UNUSED( _curl );

        //Empty
    }
    ////////////////////////////////////////////////////////////////////////
    static size_t s_writeRequestPerformerResponse( char * ptr, size_t size, size_t nmemb, void * userdata )
    {
        cURLThreadTask * perfomer = static_cast<cURLThreadTask *>(userdata);

        size_t total = size * nmemb;

        perfomer->writeResponse( ptr, total );

        return total;
    }
    //////////////////////////////////////////////////////////////////////////
    static size_t s_writeRequestHeaderResponse( char * ptr, size_t size, size_t nmemb, void * userdata )
    {
        cURLThreadTask * perfomer = static_cast<cURLThreadTask *>(userdata);
        MENGINE_UNUSED( perfomer );

        size_t total = size * nmemb;

        perfomer->writeHeader( ptr, total );
        
        return total;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::setupWriteResponse( CURL * _curl )
    {
        /* send all data to this function  */
        CURLCALL( curl_easy_setopt, (_curl, CURLOPT_WRITEDATA, (void *)this) );
        CURLCALL( curl_easy_setopt, (_curl, CURLOPT_WRITEFUNCTION, &s_writeRequestPerformerResponse) );

        if( m_receiveHeaders == true )
        {
            CURLCALL( curl_easy_setopt, (_curl, CURLOPT_HEADERDATA, (void *)this) );
            CURLCALL( curl_easy_setopt, (_curl, CURLOPT_HEADERFUNCTION, &s_writeRequestHeaderResponse) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::writeResponse( Char * _ptr, size_t _size )
    {
        m_responseData.append( _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::writeHeader( Char * _ptr, size_t _size )
    {
        m_responseHeaders.emplace_back( _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::_onComplete( bool _successful )
    {
        m_receiver->onHttpRequestComplete( m_id, (uint32_t)m_responseStatus, m_responseError, m_responseHeaders, m_responseData, m_responseCode, _successful );
        m_receiver = nullptr;
    }
}
   
