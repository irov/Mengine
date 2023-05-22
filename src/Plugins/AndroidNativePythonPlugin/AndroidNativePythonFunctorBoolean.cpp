#include "AndroidNativePythonFunctorBoolean.h"

#include "Interface/AndroidEnvironmentServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Kernel/Error.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonFunctorBoolean::AndroidNativePythonFunctorBoolean()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonFunctorBoolean::~AndroidNativePythonFunctorBoolean()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * AndroidNativePythonFunctorBoolean::call( PyObject * _args, PyObject * _kwds )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return m_kernel->ret_none();
        }

        jobject functor = this->getJavaFunctor();

        jclass jclass_FunctorBoolean = jenv->GetObjectClass( functor );

        jmethodID jmethodID_FunctorBoolean_call = jenv->GetMethodID( jclass_FunctorBoolean, "call", "(java/lang/Boolean)V" );

        jenv->CallVoidMethod( functor, jmethodID_FunctorBoolean_call );

        Helper::jEnvExceptionCheck( jenv );

        jenv->DeleteLocalRef( jclass_FunctorBoolean );

        return m_kernel->ret_none();
    }
    //////////////////////////////////////////////////////////////////////////
}
