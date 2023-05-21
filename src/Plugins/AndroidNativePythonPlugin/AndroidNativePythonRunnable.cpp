#include "AndroidNativePythonRunnable.h"

#include "Interface/AndroidEnvironmentServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Kernel/Error.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonRunnable::AndroidNativePythonRunnable()
        : m_responseCb(nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonRunnable::~AndroidNativePythonRunnable()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return;
        }

        if( m_responseCb != nullptr )
        {
            jenv->DeleteGlobalRef( m_responseCb );
            m_responseCb = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonRunnable::setResponseCb( jobject _responseCb )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return;
        }

        m_responseCb = jenv->NewGlobalRef( _responseCb );
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidNativePythonRunnable::getResponseCb() const
    {
        return m_responseCb;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonRunnable::run()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return;
        }

        jclass jclass_Runnable = jenv->GetObjectClass( m_responseCb );

        jmethodID jmethodID_Consumer_callback = jenv->GetMethodID( jclass_Runnable, "run", "()V" );

        jenv->CallVoidMethod( m_responseCb, jmethodID_Consumer_callback );

        Helper::jEnvExceptionCheck( jenv );

        jenv->DeleteLocalRef( jclass_Runnable );
    }
    //////////////////////////////////////////////////////////////////////////
}
