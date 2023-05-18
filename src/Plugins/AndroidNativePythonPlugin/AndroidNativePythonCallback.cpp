#include "AndroidNativePythonCallback.h"

#include "Interface/AndroidEnvironmentServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Kernel/Error.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonCallback::AndroidNativePythonCallback()
        : m_responseCb(nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonCallback::~AndroidNativePythonCallback()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonCallback::setResponseCb( jobject _responseCb )
    {
        m_responseCb = _responseCb;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidNativePythonCallback::getResponseCb() const
    {
        return m_responseCb;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonCallback::callback( const pybind::object & _result )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return;
        }

        jobject jresult;

        if( _result.is_none() == true )
        {
            jclass jclass_Object = jenv->FindClass( "java/lang/Object" );

            jmethodID constructor = jenv->GetMethodID( jclass_Object, "<init>", "()V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] ()V'" );

            jresult = jenv->NewObject( jclass_Object, constructor );

            jenv->DeleteLocalRef( jclass_Object );
        }
        else if( _result.is_bool() == true )
        {
            bool value = _result.extract();

            jresult = Helper::makeJObjectBoolean( jenv, value );
        }
        else if( _result.is_integer() == true )
        {
            int32_t value = _result.extract();

            jresult = Helper::makeJObjectInteger( jenv, value );
        }
        else if( _result.is_long() == true )
        {
            int64_t value = _result.extract();

            jresult = Helper::makeJObjectLong( jenv, value );
        }
        else if( _result.is_float() == true )
        {
            double value = _result.extract();

            jresult = Helper::makeJObjectDouble( jenv, value );
        }
        else if( _result.is_string() == true )
        {
            const Char * value_str = (const Char *)_result.extract();

            jresult = Helper::makeJObjectString( jenv, value_str );
        }
        else
        {
            jclass jclass_MengineCallbackInterface = jenv->GetObjectClass( m_responseCb );

            jmethodID jmethodID_MengineCallbackInterface_toString = jenv->GetMethodID(jclass_MengineCallbackInterface, "toString", "()Ljava/lang/String;");

            jstring jresponseCbToString = (jstring)jenv->CallObjectMethod( m_responseCb, jmethodID_MengineCallbackInterface_toString );

            const Char * jresponseCbToString_str = jenv->GetStringUTFChars( jresponseCbToString, nullptr );

            MENGINE_ERROR_FATAL( "android response call '%s' unsupported result type [%s]"
                , jresponseCbToString_str
                , _result.repr_type().c_str()
            );

            jenv->ReleaseStringUTFChars( jresponseCbToString, jresponseCbToString_str );

            jenv->DeleteLocalRef( jresponseCbToString );
            jenv->DeleteLocalRef( jclass_MengineCallbackInterface );

            return;
        }

        jclass jclass_MengineCallbackInterface = jenv->GetObjectClass( m_responseCb );

        jmethodID jmethodID_MengineCallbackInterface_callback = jenv->GetMethodID( jclass_MengineCallbackInterface, "callback", "(Ljava/lang/Object;)V" );

        jenv->CallVoidMethod( m_responseCb, jmethodID_MengineCallbackInterface_callback, jresult );

        Helper::jEnvExceptionCheck( jenv );

        jenv->DeleteLocalRef( jclass_MengineCallbackInterface );
        jenv->DeleteLocalRef( jresult );

        jenv->DeleteGlobalRef( m_responseCb );
        m_responseCb = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
