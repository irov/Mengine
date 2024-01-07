#include "AndroidSemaphoreListener.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidSemaphoreListener::AndroidSemaphoreListener( JNIEnv * _jenv, jobject _cb )
        : m_cb( _jenv->NewGlobalRef( _cb ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidSemaphoreListener::~AndroidSemaphoreListener()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jenv->DeleteGlobalRef( m_cb );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSemaphoreListener::onActivated()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jclass jclass_Listener = jenv->GetObjectClass( m_cb );

        jmethodID jclass_Listener_call = jenv->GetMethodID( jclass_Listener, "call", "()V" );

        jenv->CallVoidMethod( m_cb, jclass_Listener_call );

        Helper::jEnvExceptionCheck( jenv );

        jenv->DeleteLocalRef( jclass_Listener );
    }
    //////////////////////////////////////////////////////////////////////////
}

