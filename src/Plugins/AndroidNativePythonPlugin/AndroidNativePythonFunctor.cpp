#include "AndroidNativePythonFunctor.h"

#include "Interface/AndroidEnvironmentServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Kernel/Error.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonFunctor::AndroidNativePythonFunctor()
        : m_kernel( nullptr )
        , m_functor( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonFunctor::~AndroidNativePythonFunctor()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return;
        }

        if( m_functor != nullptr )
        {
            jenv->DeleteGlobalRef( m_functor );
            m_functor = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonFunctor::setKernel( pybind::kernel_interface * _kernel )
    {
        m_kernel = _kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::kernel_interface * AndroidNativePythonFunctor::getKernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonFunctor::setJavaFunctor( jobject _functor )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return;
        }

        m_functor = jenv->NewGlobalRef( _functor );
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidNativePythonFunctor::getJavaFunctor() const
    {
        return m_functor;
    }
    //////////////////////////////////////////////////////////////////////////
}
