#include "cURLHttpRequest.h"

#include "Interface/FileGroupInterface.h"

#include "cURLHttpErrorHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/HttpLogger.h"

#include "Config/StdIntTypes.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static int cURL_trace( CURL * handle, curl_infotype type, char * data, size_t size, void * userp )
        {
            MENGINE_UNUSED( handle );
            MENGINE_UNUSED( userp );

            switch( type )
            {
            case CURLINFO_TEXT:
                {
                    LOGGER_HTTP_MESSAGE_RELEASE( "== Info, %.*s"
                        , (int32_t)size - 1
                        , data
                    );
                }break;
            case CURLINFO_HEADER_IN:
                {
                    LOGGER_HTTP_MESSAGE_RELEASE( "<= Recv header, %.*s"
                        , (int32_t)size - 1
                        , data
                    );
                }break;
            case CURLINFO_HEADER_OUT:
                {
                    LOGGER_HTTP_MESSAGE_RELEASE( "=> Send header, %.*s"
                        , (int32_t)size - 1
                        , data
                    );
                }break;
            case CURLINFO_DATA_IN:
                {
                    LOGGER_HTTP_MESSAGE_RELEASE( "<= Recv data, %zu bytes %.*s"
                        , size
                        , (int32_t)size
                        , data
                    );
                }break;
            case CURLINFO_DATA_OUT:
                {
                    LOGGER_HTTP_MESSAGE_RELEASE( "=> Send data, %zu bytes %.*s"
                        , size
                        , (int32_t)size
                        , data
                    );
                }break;
            case CURLINFO_SSL_DATA_IN:
                {
                    LOGGER_HTTP_MESSAGE_RELEASE( "<= Recv SSL data, %zu bytes %.*s"
                        , size
                        , (int32_t)size
                        , data
                    );
                }break;
            case CURLINFO_SSL_DATA_OUT:
                {
                    LOGGER_HTTP_MESSAGE_RELEASE( "=> Send SSL data, %zu bytes %.*s"
                        , size
                        , (int32_t)size
                        , data
                    );
                }break;
            case CURLINFO_END:
                {
                    //Empty
                }break;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        static size_t cURL_writeRequestPerformerResponse( char * ptr, size_t size, size_t nmemb, void * userdata )
        {
            cURLHttpRequest * task = static_cast<cURLHttpRequest *>(userdata);

            if( task->isCancel() == true )
            {
                return 0;
            }

            size_t total = size * nmemb;

            task->writeResponse( ptr, total );

            return total;
        }
        //////////////////////////////////////////////////////////////////////////
        static size_t cURL_writeRequestHeaderResponse( char * ptr, size_t size, size_t nmemb, void * userdata )
        {
            cURLHttpRequest * task = static_cast<cURLHttpRequest *>(userdata);

            if( task->isCancel() == true )
            {
                return 0;
            }

            size_t total = size * nmemb;

            task->writeHeader( ptr, total );

            return total;
        }
        //////////////////////////////////////////////////////////////////////////
        static int32_t cURL_XFERInfoCallback( void * _userp, curl_off_t _dltotal, curl_off_t _dlnow, curl_off_t _ultotal, curl_off_t _ulnow )
        {
            MENGINE_UNUSED( _dltotal );
            MENGINE_UNUSED( _dlnow );
            MENGINE_UNUSED( _ultotal );
            MENGINE_UNUSED( _ulnow );

            cURLHttpRequest * task = static_cast<cURLHttpRequest *>(_userp);

            if( task->isCancel() == true )
            {
                return 1;
            }

            return 0;
        }
        ////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequest::cURLHttpRequest()
        : m_curl_header_list( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequest::~cURLHttpRequest()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequest::_onThreadTaskPreparation()
    {
        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHttpRequest::_onThreadTaskRun()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHttpRequest::_onThreadTaskProcess()
    {
        MENGINE_PROFILER_CATEGORY();

        MENGINE_ASSERTION_MEMORY_PANIC( m_response, "not setup 'response'" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_receiver, "not setup 'receiver'" );

        CURL * curl = curl_easy_init();

        const Char * url_str = m_url.c_str();

        MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_URL, url_str) );
        //CURLCALL( curl_easy_setopt, (curl, CURLOPT_FOLLOWLOCATION, 1) );

        if( m_proxy.empty() == false )
        {
            const Char * proxy_str = m_proxy.c_str();

            MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_PROXY, proxy_str) );
        }

        struct curl_slist * curl_header_list = nullptr;

        if( m_headers.empty() == false )
        {
            for( const String & header : m_headers )
            {
                const Char * header_buffer = header.c_str();

                curl_header_list = curl_slist_append( curl_header_list, header_buffer );
            }

            MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_HTTPHEADER, curl_header_list) );

            m_curl_header_list = curl_header_list;
        }

        this->_onCURL( curl );

        if( m_timeout != MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY )
        {
            MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_TIMEOUT_MS, m_timeout) );
        }

        if( m_cookies.empty() == false )
        {
            const Char * cookies_str = m_cookies.c_str();

            MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_COOKIE, cookies_str) );
        }

        MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_SSL_VERIFYPEER, 0) );
        MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_SSL_VERIFYHOST, 0) );

        Char errorbuf[CURL_ERROR_SIZE] = {'\0'};
        MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_ERRORBUFFER, errorbuf) );

        bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        if( OPTION_curltrace == true )
        {
            MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_VERBOSE, 1) );

            MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_DEBUGDATA, nullptr) );
            MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_DEBUGFUNCTION, &Detail::cURL_trace) );
        }

        MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_XFERINFODATA, (void *)this) );
        MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_XFERINFOFUNCTION, &Detail::cURL_XFERInfoCallback) );

        MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_NOPROGRESS, 0L) );

        CURLcode status = curl_easy_perform( curl );

        m_mutex->lock();

        if( m_response != nullptr )
        {
            if( status != CURLE_OK )
            {
                m_response->setError( errorbuf );
            }

            long http_code = 0;
            MENGINE_CURLCALL( curl_easy_getinfo, (curl, CURLINFO_RESPONSE_CODE, &http_code) );

            m_response->setCode( (EHttpCode)http_code );
        }

        m_mutex->unlock();

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
    void cURLHttpRequest::_onCURLCleanup( CURL * _curl )
    {
        MENGINE_UNUSED( _curl );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequest::setupWriteResponse( CURL * _curl )
    {
        MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_WRITEDATA, (void *)this) );
        MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_WRITEFUNCTION, &Detail::cURL_writeRequestPerformerResponse) );

        if( m_receiveHeaders == true )
        {
            MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_HEADERDATA, (void *)this) );
            MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_HEADERFUNCTION, &Detail::cURL_writeRequestHeaderResponse) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequest::writeResponse( const Char * _ptr, size_t _size )
    {
        m_mutex->lock();

        if( m_response != nullptr )
        {
            m_response->appendData( _ptr, _size );
        }

        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequest::writeHeader( const Char * _ptr, size_t _size )
    {
        m_mutex->lock();

        if( m_response != nullptr )
        {
            m_response->appendHeaders( _ptr, _size );
        }

        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequest::_onThreadTaskComplete( bool _successful )
    {
        m_response->setSuccessful( _successful );

        m_mutex->lock();

        HttpResponseInterfacePtr response = std::move( m_response );
        HttpReceiverInterfacePtr receiver = std::move( m_receiver );
        receiver->onHttpRequestComplete( response );

        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
}
