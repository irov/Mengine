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
    void AndroidNativePythonFunctorBoolean::call( bool _result )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return;
        }

        jobject functor = this->getJavaFunctor();

        jclass jclass_FunctorBoolean = jenv->GetObjectClass( functor );

        jmethodID jmethodID_FunctorBoolean_call = jenv->GetMethodID( jclass_FunctorBoolean, "call", "(Z)V" );

        jenv->CallVoidMethod( functor, jmethodID_FunctorBoolean_call, _result );

        Helper::jEnvExceptionCheck( jenv );

        jenv->DeleteLocalRef( jclass_FunctorBoolean );
    }
    //////////////////////////////////////////////////////////////////////////
}
