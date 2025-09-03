#include "AndroidNativePythonCallback.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Kernel/Error.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonCallback::AndroidNativePythonCallback()
        : m_kernel( nullptr )
        , m_functor( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonCallback::~AndroidNativePythonCallback()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        if( m_functor != nullptr )
        {
            Mengine_JNI_DeleteGlobalRef( jenv, m_functor );
            m_functor = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonCallback::setKernel( pybind::kernel_interface * _kernel )
    {
        m_kernel = _kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::kernel_interface * AndroidNativePythonCallback::getKernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonCallback::setJavaFunctor( JNIEnv * _jenv, jobject _functor )
    {
        if( m_functor != nullptr )
        {
            Mengine_JNI_DeleteGlobalRef( _jenv, m_functor );
        }

        m_functor = Mengine_JNI_NewGlobalRef( _jenv, _functor );
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidNativePythonCallback::getJavaFunctor() const
    {
        return m_functor;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonCallback::call( bool _result, const Params & _params )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jobject jobject_functor = this->getJavaFunctor();

        jclass jclass_Functor = Mengine_JNI_GetObjectClass( jenv, jobject_functor );

        jmethodID jmethodID_FunctorBoolean_call = Mengine_JNI_GetMethodID( jenv, jclass_Functor, "call", "(ZLjava/util/Map;)V" );

        jobject jobject_params = Helper::AndroidMakeJObjectHashMap( jenv, _params );

        Mengine_JNI_CallVoidMethod( jenv, jobject_functor, jmethodID_FunctorBoolean_call, (_result == true ? JNI_TRUE : JNI_FALSE), jobject_params );

        Helper::AndroidEnvExceptionCheck( jenv );

        Mengine_JNI_DeleteLocalRef( jenv, jobject_params );
        Mengine_JNI_DeleteLocalRef( jenv, jclass_Functor );
    }
    //////////////////////////////////////////////////////////////////////////
}
