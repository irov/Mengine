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
    AndroidHttpRequest::AndroidHttpRequest()
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

        jobject jheaders = Helper::AndroidMakeJObjectArrayList( jenv, headers_size );

        for( const String & header : m_headers )
        {
            jobject jheader = Helper::AndroidMakeJObjectString( jenv, header );

            Helper::AndroidAddJObjectList( jenv, jheaders, jheader );

            jenv->DeleteLocalRef( jheader );
        }

        jobject jcookies = Helper::AndroidMakeJObjectString( jenv, m_cookies );

        jclass jclass_MengineHttpRequestParam = Mengine::Mengine_JNI_FindClass( jenv, "org/Mengine/Base/MengineHttpRequestParam" );

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

        if( jresponse == nullptr )
        {
            m_mutex->lock();

            if( m_response != nullptr )
            {
                m_response->setSuccessful( false );
            }

            m_mutex->unlock();

            return true;
        }

        jclass jclass_MengineHttpResponseParam = Mengine::Mengine_JNI_FindClass( jenv, "org/Mengine/Base/MengineHttpResponseParam" );

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
            m_response->setSuccessful( true );

            m_response->setCode( (EHttpCode)HTTP_RESPONSE_CODE );

            if( HTTP_CONTENT_DATA != nullptr )
            {
                jbyte * elements = jenv->GetByteArrayElements( HTTP_CONTENT_DATA, NULL );
                m_response->appendData( (Char *)elements, HTTP_CONTENT_LENGTH );
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
            jenv->DeleteLocalRef( HTTP_CONTENT_DATA );
        }

        if( HTTP_ERROR_MESSAGE != nullptr )
        {
            jenv->DeleteLocalRef( HTTP_ERROR_MESSAGE );
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
