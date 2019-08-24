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
        , m_code( 0 )
        , m_status( CURLE_OK )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLThreadTask::~cURLThreadTask()
    {
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

        m_status = status;
        m_error = errorbuf;

        long http_code = 0;
        CURLCALL( curl_easy_getinfo, (curl, CURLINFO_RESPONSE_CODE, &http_code) );

        m_code = (uint32_t)http_code;

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
    void cURLThreadTask::setupWriteResponse( CURL * _curl )
    {
        /* send all data to this function  */
        CURLCALL( curl_easy_setopt, (_curl, CURLOPT_WRITEDATA, (void *)this) );
        CURLCALL( curl_easy_setopt, (_curl, CURLOPT_WRITEFUNCTION, &s_writeRequestPerformerResponse) );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::writeResponse( Char * _ptr, size_t _size )
    {
        m_response.append( _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLThreadTask::_onComplete( bool _successful )
    {
        m_receiver->onHttpRequestComplete( m_id, (uint32_t)m_status, m_error, m_response, m_code, _successful );
        m_receiver = nullptr;
    }
}
   