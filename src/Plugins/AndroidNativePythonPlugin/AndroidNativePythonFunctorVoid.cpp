#include "AndroidNativePythonFunctorVoid.h"

#include "Interface/AndroidEnvironmentServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Kernel/Error.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonFunctorVoid::AndroidNativePythonFunctorVoid()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonFunctorVoid::~AndroidNativePythonFunctorVoid()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * AndroidNativePythonFunctorVoid::call( PyObject * _args, PyObject * _kwds )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return m_kernel->ret_none();
        }

        jobject functor = this->getJavaFunctor();

        jclass jclass_Functor = jenv->GetObjectClass( functor );

        jmethodID jmethodID_Functor_call = jenv->GetMethodID( jclass_Functor, "call", "()V" );

        jenv->CallVoidMethod( jclass_Functor, jmethodID_Functor_call );

        Helper::jEnvExceptionCheck( jenv );

        jenv->DeleteLocalRef( jclass_Functor );

        return m_kernel->ret_none();
    }
    //////////////////////////////////////////////////////////////////////////
}
