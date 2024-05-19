#include "AndroidHttpRequestThreadTask.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdIntTypes.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestThreadTask::AndroidHttpRequestThreadTask()
        : m_id( 0 )
        , m_timeout( MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY )
        , m_receiveHeaders( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestThreadTask::~AndroidHttpRequestThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestThreadTask::setURL( const String & _url )
    {
        m_url = _url;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & AndroidHttpRequestThreadTask::getURL() const
    {
        return m_url;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestThreadTask::setProxy( const String & _proxy )
    {
        m_proxy = _proxy;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & AndroidHttpRequestThreadTask::getProxy() const
    {
        return m_proxy;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestThreadTask::setCookies( const String & _cookies )
    {
        m_cookies = _cookies;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & AndroidHttpRequestThreadTask::getCookies() const
    {
        return m_cookies;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestThreadTask::setHeaders( const HttpRequestHeaders & _headers )
    {
        m_headers = _headers;
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpRequestHeaders & AndroidHttpRequestThreadTask::getHeaders() const
    {
        return m_headers;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestThreadTask::setRequestId( HttpRequestId _id )
    {
        m_id = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId AndroidHttpRequestThreadTask::getRequestId() const
    {
        return m_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestThreadTask::setTimeout( int32_t _timeout )
    {
        m_timeout = _timeout;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidHttpRequestThreadTask::getTimeout() const
    {
        return m_timeout;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestThreadTask::setReceiveHeaders( bool _receiveHeaders )
    {
        m_receiveHeaders = _receiveHeaders;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidHttpRequestThreadTask::getReceiveHeaders() const
    {
        return m_receiveHeaders;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestThreadTask::setReponse( const HttpResponseInterfacePtr & _response )
    {
        m_response = _response;
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpResponseInterfacePtr & AndroidHttpRequestThreadTask::getReponse() const
    {
        return m_response;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestThreadTask::setReceiver( const HttpReceiverInterfacePtr & _receiver )
    {
        m_receiver = _receiver;
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpReceiverInterfacePtr & AndroidHttpRequestThreadTask::getReceiver() const
    {
        return m_receiver;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestThreadTask::_onThreadTaskPreparation()
    {
        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidHttpRequestThreadTask::_onThreadTaskRun()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidHttpRequestThreadTask::_onThreadTaskProcess()
    {
        MENGINE_PROFILER_CATEGORY();

        MENGINE_ASSERTION_MEMORY_PANIC( m_response, "not setup 'respose'" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_receiver, "not setup 'receiver'" );

        //CURL * curl = curl_easy_init();

        //const Char * url_str = m_url.c_str();

        //MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_URL, url_str) );
        //MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_FOLLOWLOCATION, 1) );

        //if( m_proxy.empty() == false )
        //{
        //    const Char * proxy_str = m_proxy.c_str();

        //    MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_PROXY, proxy_str) );
        //}

        //struct curl_slist * curl_header_list = nullptr;

        //if( m_headers.empty() == false )
        //{
        //    for( const String & header : m_headers )
        //    {
        //        const Char * header_buffer = header.c_str();

        //        curl_header_list = curl_slist_append( curl_header_list, header_buffer );
        //    }

        //    MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_HTTPHEADER, curl_header_list) );

        //    m_curl_header_list = curl_header_list;
        //}

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jobject jurl = Helper::AndroidMakeJObjectString( jenv, m_url );
        jobject jproxy = Helper::AndroidMakeJObjectString( jenv, m_proxy );

        HttpRequestHeaders::size_type headers_size = m_headers.size();

        jobject jheaders = Helper::AndroidMakeJObjectArrayList( jenv, headers_size );

        for( const String & header : m_headers )
        {
            jobject jheader = Helper::AndroidMakeJObjectString( jenv, header );

            Helper::AndroidAddJObjectList( jenv, jheaders, jheader );

            jenv->DeleteLocalRef( jheader );
        }

        jobject jcookies = Helper::AndroidMakeJObjectString( jenv, m_cookies );

        jclass jclass_MengineHttpRequestParam = ::Mengine_JNI_FindClass( jenv, "org/Mengine/Base/MengineHttpRequestParam" );

        Helper::AndroidEnvExceptionCheck( jenv );

        jmethodID jmethod_MengineHttpRequestParam_constructor = jenv->GetMethodID( jclass_MengineHttpRequestParam, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/util/List;Ljava/lang/String;I)V" );

        jobject jrequest = jenv->NewObject( jclass_MengineHttpRequestParam, jmethod_MengineHttpRequestParam_constructor
            , jurl
            , jproxy
            , jheaders
            , jcookies
            , m_timeout
        );

        jenv->DeleteLocalRef( jclass_MengineHttpRequestParam );

        jenv->DeleteLocalRef( jurl );
        jenv->DeleteLocalRef( jproxy );
        jenv->DeleteLocalRef( jheaders );
        jenv->DeleteLocalRef( jcookies );

        jobject jresponse = this->_onHttp( jenv, jrequest );

        jenv->DeleteLocalRef( jrequest );

        jclass jclass_MengineHttpResponseParam = ::Mengine_JNI_FindClass( jenv, "org/Mengine/Base/MengineHttpResponseParam" );

        Helper::AndroidEnvExceptionCheck( jenv );

        jfieldID jfieldiD_HTTP_RESPONSE_CODE = jenv->GetFieldID( jclass_MengineHttpResponseParam, "HTTP_RESPONSE_CODE", "I" );
        jfieldID jfieldiD_HTTP_CONTENT_LENGTH = jenv->GetFieldID( jclass_MengineHttpResponseParam, "HTTP_CONTENT_LENGTH", "I" );
        jfieldID jfieldiD_HTTP_CONTENT_DATA = jenv->GetFieldID( jclass_MengineHttpResponseParam, "HTTP_CONTENT_DATA", "[B" );
        jfieldID jfieldiD_HTTP_ERROR_MESSAGE = jenv->GetFieldID( jclass_MengineHttpResponseParam, "HTTP_ERROR_MESSAGE", "Ljava/lang/String;" );

        jenv->DeleteLocalRef( jclass_MengineHttpResponseParam );

        int HTTP_RESPONSE_CODE = jenv->GetIntField( jresponse, jfieldiD_HTTP_RESPONSE_CODE );
        int HTTP_CONTENT_LENGTH = jenv->GetIntField( jresponse, jfieldiD_HTTP_CONTENT_LENGTH );
        jbyteArray HTTP_CONTENT_DATA = (jbyteArray)jenv->GetObjectField( jresponse, jfieldiD_HTTP_CONTENT_DATA );
        jstring HTTP_ERROR_MESSAGE = (jstring)jenv->GetObjectField( jresponse, jfieldiD_HTTP_ERROR_MESSAGE );

        jenv->DeleteLocalRef( jresponse );

        m_mutex->lock();

        if( m_response != nullptr )
        {
            m_response->setCode( (EHttpCode)HTTP_RESPONSE_CODE );

            if( HTTP_CONTENT_DATA != nullptr )
            {
                jbyte * elements = jenv->GetByteArrayElements( HTTP_CONTENT_DATA, NULL );
                m_response->appendData( (Char *)elements, HTTP_CONTENT_LENGTH );
            }

            if( HTTP_ERROR_MESSAGE != nullptr )
            {
                String error_message = Helper::AndroidMakeStringFromJString( jenv, HTTP_ERROR_MESSAGE );
                m_response->setError( error_message );
            }
        }

        m_mutex->unlock();

        if( HTTP_CONTENT_DATA != nullptr )
        {
            jenv->DeleteLocalRef( HTTP_CONTENT_DATA );
        }

        if( HTTP_ERROR_MESSAGE != nullptr )
        {
            jenv->DeleteLocalRef( HTTP_ERROR_MESSAGE );
        }

        //if( m_timeout != MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY )
        //{
        //    MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_TIMEOUT_MS, m_timeout) );
        //}

        //if( m_cookies.empty() == false )
        //{
        //    const Char * cookies_str = m_cookies.c_str();

        //    MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_COOKIE, cookies_str) );
        //}

        //MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_SSL_VERIFYPEER, 0) );
        //MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_SSL_VERIFYHOST, 0) );

        //Char errorbuf[CURL_ERROR_SIZE] = {'\0'};
        //MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_ERRORBUFFER, errorbuf) );

        //bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        //if( OPTION_curltrace == true )
        //{
        //    MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_VERBOSE, 1) );

        //    MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_DEBUGDATA, nullptr) );
        //    MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_DEBUGFUNCTION, &Detail::cURL_trace) );
        //}

        //MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_XFERINFODATA, (void *)this) );
        //MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_XFERINFOFUNCTION, &Detail::cURL_XFERInfoCallback) );

        //MENGINE_CURLCALL( curl_easy_setopt, (curl, CURLOPT_NOPROGRESS, 0L) );

        //CURLcode status = curl_easy_perform( curl );

        //m_mutex->lock();

        //if( m_response != nullptr )
        //{
            //m_response->setError( errorbuf );

            //long http_code = 0;
            //MENGINE_CURLCALL( curl_easy_getinfo, (curl, CURLINFO_RESPONSE_CODE, &http_code) );

            //m_response->setCode( (EHttpCode)http_code );
        //}

        //m_mutex->unlock();

        //if( m_curl_header_list != nullptr )
        //{
        //    curl_slist_free_all( m_curl_header_list );
        //    m_curl_header_list = nullptr;
        //}

        //this->_onCURLCleanup( curl );

        //curl_easy_cleanup( curl );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestThreadTask::_onThreadTaskComplete( bool _successful )
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
