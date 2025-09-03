#include "AndroidHttpRequest.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdIntTypes.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequest::AndroidHttpRequest( bool _isJSON )
        : m_isJSON( _isJSON )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequest::~AndroidHttpRequest()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequest::_onThreadTaskPreparation()
    {
        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex" );

        m_mutex = mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidHttpRequest::_onThreadTaskRun()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidHttpRequest::_onThreadTaskProcess()
    {
        MENGINE_PROFILER_CATEGORY();

        MENGINE_ASSERTION_MEMORY_PANIC( m_response, "not setup 'response'" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_receiver, "not setup 'receiver'" );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jobject jurl = Helper::AndroidMakeJObjectString( jenv, m_url );
        jobject jproxy = Helper::AndroidMakeJObjectString( jenv, m_proxy );

        HttpHeaders::size_type headers_size = m_headers.size();

        jobject jobject_headers = Helper::AndroidMakeJObjectArrayList( jenv, headers_size );

        for( const String & header : m_headers )
        {
            jobject jobject_header = Helper::AndroidMakeJObjectString( jenv, header );

            Helper::AndroidAddJObjectList( jenv, jobject_headers, jobject_header );

            Mengine_JNI_DeleteLocalRef( jenv, jobject_header );
        }

        jobject jobject_cookies = Helper::AndroidMakeJObjectString( jenv, m_cookies );

        jclass jclass_MengineHttpRequestParam = Helper::AndroidEnvFindClass( jenv, "org/Mengine/Base/MengineParamHttpRequest" );

        jmethodID jmethod_MengineHttpRequestParam_constructor = Mengine_JNI_GetMethodID( jenv, jclass_MengineHttpRequestParam, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/util/List;Ljava/lang/String;I)V" );

        jobject jobject_request = Mengine_JNI_NewObject( jenv, jclass_MengineHttpRequestParam, jmethod_MengineHttpRequestParam_constructor
            , jurl
            , jproxy
            , jobject_headers
            , jobject_cookies
            , m_timeout
        );

        Helper::AndroidEnvExceptionCheck( jenv );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_MengineHttpRequestParam );

        Mengine_JNI_DeleteLocalRef( jenv, jurl );
        Mengine_JNI_DeleteLocalRef( jenv, jproxy );
        Mengine_JNI_DeleteLocalRef( jenv, jobject_headers );
        Mengine_JNI_DeleteLocalRef( jenv, jobject_cookies );

        jobject jobject_response = this->_onHttp( jenv, jobject_request );

        Mengine_JNI_DeleteLocalRef( jenv, jobject_request );

        if( jobject_response == nullptr )
        {
            m_mutex->lock();

            if( m_response != nullptr )
            {
                m_response->setSuccessful( false );
            }

            m_mutex->unlock();

            return true;
        }

        jclass jclass_MengineHttpResponseParam = Helper::AndroidEnvFindClass( jenv, "org/Mengine/Base/MengineParamHttpResponse" );

        jfieldID jfieldiD_HTTP_RESPONSE_CODE = Mengine_JNI_GetFieldID( jenv, jclass_MengineHttpResponseParam, "HTTP_RESPONSE_CODE", "I" );
        jfieldID jfieldiD_HTTP_CONTENT_LENGTH = Mengine_JNI_GetFieldID( jenv, jclass_MengineHttpResponseParam, "HTTP_CONTENT_LENGTH", "I" );
        jfieldID jfieldiD_HTTP_CONTENT_DATA = Mengine_JNI_GetFieldID( jenv, jclass_MengineHttpResponseParam, "HTTP_CONTENT_DATA", "[B" );
        jfieldID jfieldiD_HTTP_ERROR_MESSAGE = Mengine_JNI_GetFieldID( jenv, jclass_MengineHttpResponseParam, "HTTP_ERROR_MESSAGE", "Ljava/lang/String;" );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_MengineHttpResponseParam );

        int HTTP_RESPONSE_CODE = Mengine_JNI_GetIntField( jenv, jobject_response, jfieldiD_HTTP_RESPONSE_CODE );
        int HTTP_CONTENT_LENGTH = Mengine_JNI_GetIntField( jenv, jobject_response, jfieldiD_HTTP_CONTENT_LENGTH );
        jbyteArray HTTP_CONTENT_DATA = (jbyteArray)Mengine_JNI_GetObjectField( jenv, jobject_response, jfieldiD_HTTP_CONTENT_DATA );
        jstring HTTP_ERROR_MESSAGE = (jstring)Mengine_JNI_GetObjectField( jenv, jobject_response, jfieldiD_HTTP_ERROR_MESSAGE );

        Mengine_JNI_DeleteLocalRef( jenv, jobject_response );

        m_mutex->lock();

        if( m_response != nullptr )
        {
            m_response->setSuccessful( true );

            m_response->setCode( (EHttpCode)HTTP_RESPONSE_CODE );

            if( HTTP_CONTENT_DATA != nullptr )
            {
                jbyte * bytes = Mengine_JNI_GetByteArrayElements( jenv, HTTP_CONTENT_DATA, NULL );

                if( m_isJSON == true )
                {
                    m_response->appendJSON( bytes, HTTP_CONTENT_LENGTH );
                }
                else
                {
                    m_response->appendData( bytes, HTTP_CONTENT_LENGTH );
                }

                Mengine_JNI_ReleaseByteArrayElements( jenv, HTTP_CONTENT_DATA, bytes, 0 );
            }

            if( HTTP_ERROR_MESSAGE != nullptr )
            {
                String error_message = Helper::AndroidMakeStringFromJString( jenv, HTTP_ERROR_MESSAGE );

                m_response->setError( error_message, 0 );
            }
        }

        m_mutex->unlock();

        if( HTTP_CONTENT_DATA != nullptr )
        {
            Mengine_JNI_DeleteLocalRef( jenv, HTTP_CONTENT_DATA );
        }

        if( HTTP_ERROR_MESSAGE != nullptr )
        {
            Mengine_JNI_DeleteLocalRef( jenv, HTTP_ERROR_MESSAGE );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequest::_onThreadTaskComplete( bool _successful )
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
