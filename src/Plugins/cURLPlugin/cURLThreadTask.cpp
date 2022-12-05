#include "cURLThreadTask.h"

#include "Interface/FileGroupInterface.h"

#include "cURLErrorHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

#include "Config/StdIntTypes.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static int cURL_trace( CURL * handle, curl_infotype type, char * data, size_t size, void * userp )
        {
            MENGINE_UNUSED( handle );
            MENGINE_UNUSED( data );
            MENGINE_UNUSED( size );
            MENGINE_UNUSED( userp );

            const Char * text;

            switch( type )
            {
            case CURLINFO_TEXT:
                LOGGER_MESSAGE( "== Info: %s", data );
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

            LOGGER_MESSAGE( "%s, %10.10zu bytes (0x%8.8zu)"
                , text, size, size
            );

            LOGGER_MESSAGE( "%.*s"
                , (int32_t)size
                , data 
            );

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        static size_t cURL_writeRequestPerformerResponse( char * ptr, size_t size, size_t nmemb, void * userdata )
        {
            cURLThreadTask * perfomer = static_cast<cURLThreadTask *>(userdata);

            size_t total = size * nmemb;

            perfomer->writeResponse( ptr, total );

            return total;
        }
        //////////////////////////////////////////////////////////////////////////
        static size_t cURL_writeRequestHeaderResponse( char * ptr, size_t size, size_t nmemb, void * userdata )
        {
            cURLThreadTask * perfomer = static_cast<cURLThreadTask *>(userdata);
            MENGINE_UNUSED( perfomer );

            size_t total = size * nmemb;

            perfomer->writeHeader( ptr, total );

            return total;
        }
        //////////////////////////////////////////////////////////////////////////
        static int32_t cURL_XFERInfoCallback( void * _userp, curl_off_t _dltotal, curl_off_t _dlnow, curl_off_t _ultotal, curl_off_t _ulnow )
        {
            MENGINE_UNUSED( _dltotal );
            MENGINE_UNUSED( _dlnow );
            MENGINE_UNUSED( _ultotal );
            MENGINE_UNUSED( _ulnow );

            cURLThreadTask * task = (cURLThreadTask *)_userp;

            if( task->isCancel() == true )
            {
                return 1;
            }

            return 0;
        }
        ////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    cURLThreadTask::cURLThreadTask()
        : m_id( 0 )
        , m_timeout( MENGINE_CURL_TIMEOUT_INFINITY )
        , m_receiveHeaders( false )
        , m_curl_header_list( nullptr )
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
    void cURLThreadTask::setCookies( const String & _cookies )
    {
        m_cookies = _cookies;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & cURLThreadTask::getCookies() const
    {
        return m_cookies;
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
    void cURLThreadTask::setReponse( const cURLResponsePtr & _response )
    {
        m_response = _response;
    }
    const cURLResponseInterfacePtr & cURLThreadTask::getReponse() const
    {
        return m_response;
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
    bool cURLThreadTask::_onThreadTaskRun()
    {
        //Empty
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLThreadTask::_onThreadTaskProcess()
    {
        MENGINE_PROFILER_CATEGORY();

        CURL * curl = curl_easy_init();

        const Char * url_str = m_url.c_str();

        CURLCALL( curl_easy_setopt, (curl, CURLOPT_URL, url_str) );
        //CURLCALL( curl_easy_setopt, (curl, CURLOPT_FOLLOWLOCATION, 1) );

        struct curl_slist * curl_header_list = nullptr;

        if( m_headers.empty() == false )
        {
            for( const String & header : m_headers )
            {
                const Char * header_buffer = header.c_str();

                curl_header_list = curl_slist_append( curl_header_list, header_buffer );
            }

            CURLCALL( curl_easy_setopt, (curl, CURLOPT_HTTPHEADER, curl_header_list) );

            m_curl_header_list = curl_header_list;
        }

        this->_onCURL( curl );

        if( m_timeout != MENGINE_CURL_TIMEOUT_INFINITY )
        {
            CURLCALL( curl_easy_setopt, (curl, CURLOPT_TIMEOUT_MS, m_timeout) );
        }

        if( m_cookies.empty() == false )
        {
            const Char * cookies_str = m_cookies.c_str();

            CURLCALL( curl_easy_setopt, (curl, CURLOPT_COOKIE, cookies_str) );
        }

        CURLCALL( curl_easy_setopt, (curl, CURLOPT_SSL_VERIFYPEER, 0) );
        CURLCALL( curl_easy_setopt, (curl, CURLOPT_SSL_VERIFYHOST, 0) );

        Char errorbuf[CURL_ERROR_SIZE] = {'\0'};
        CURLCALL( curl_easy_setopt, (curl, CURLOPT_ERRORBUFFER, errorbuf) );

        bool OPTION_curlverbose = HAS_OPTION( "curlverbose" );

        if( OPTION_curlverbose == true )
        {
            CURLCALL( curl_easy_setopt, (curl, CURLOPT_VERBOSE, 1) );
        }

        bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        if( OPTION_curltrace == true )
        {
            CURLCALL( curl_easy_setopt, (curl, CURLOPT_DEBUGFUNCTION, &Detail::cURL_trace) );
            CURLCALL( curl_easy_setopt, (curl, CURLOPT_DEBUGDATA, nullptr) );
        }

        CURLCALL( curl_easy_setopt, (curl, CURLOPT_XFERINFOFUNCTION, &Detail::cURL_XFERInfoCallback) );
        CURLCALL( curl_easy_setopt, (curl, CURLOPT_XFERINFODATA, (void *)this) );

        CURLCALL( curl_easy_setopt, (curl, CURLOPT_NOPROGRESS, 0L) );

        CURLcode status = curl_easy_perform( curl );

        m_response->setStatus( status );
        m_response->setError( errorbuf );

        long http_code = 0;
        CURLCALL( curl_easy_getinfo, (curl, CURLINFO_RESPONSE_CODE, &http_code) );

        m_response->setCode( (uint32_t)http_code );

        if( m_curl_header_list != nullptr )
        {
            curl_slist_free_all( m_curl_header_list );
            m_curl_header_list = nullptr;
        }

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
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::setupWriteResponse( CURL * _curl )
    {
        CURLCALL( curl_easy_setopt, (_curl, CURLOPT_WRITEDATA, (void *)this) );
        CURLCALL( curl_easy_setopt, (_curl, CURLOPT_WRITEFUNCTION, &Detail::cURL_writeRequestPerformerResponse) );

        if( m_receiveHeaders == true )
        {
            CURLCALL( curl_easy_setopt, (_curl, CURLOPT_HEADERDATA, (void *)this) );
            CURLCALL( curl_easy_setopt, (_curl, CURLOPT_HEADERFUNCTION, &Detail::cURL_writeRequestHeaderResponse) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::writeResponse( const Char * _ptr, size_t _size )
    {
        m_response->appendData( _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::writeHeader( const Char * _ptr, size_t _size )
    {
        m_response->appendHeaders( _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::_onThreadTaskComplete( bool _successful )
    {
        m_response->setSuccessful( _successful );

        cURLResponsePtr response = std::move( m_response );
        cURLReceiverInterfacePtr receiver = std::move( m_receiver );
        receiver->onHttpRequestComplete( response );
    }
    //////////////////////////////////////////////////////////////////////////
}
