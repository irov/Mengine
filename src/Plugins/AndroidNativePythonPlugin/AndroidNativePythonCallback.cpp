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
            jenv->DeleteGlobalRef( m_functor );
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
            _jenv->DeleteGlobalRef( m_functor );
        }

        m_functor = _jenv->NewGlobalRef( _functor );
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

        jobject jfunctor = this->getJavaFunctor();

        jclass jclass_Functor = jenv->GetObjectClass( jfunctor );

        jmethodID jmethodID_FunctorBoolean_call = jenv->GetMethodID( jclass_Functor, "call", "(ZLjava/util/Map;)V" );

        jobject jparams = Helper::AndroidMakeJObjectHashMap( jenv, _params );

        jenv->CallVoidMethod( jfunctor, jmethodID_FunctorBoolean_call, (_result == true ? JNI_TRUE : JNI_FALSE), jparams );

        Helper::AndroidEnvExceptionCheck( jenv );

        jenv->DeleteLocalRef( jparams );
        jenv->DeleteLocalRef( jclass_Functor );
    }
    //////////////////////////////////////////////////////////////////////////
}
