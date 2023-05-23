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
    void AndroidNativePythonFunctorVoid::call()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return;
        }

        jobject functor = this->getJavaFunctor();

        jclass jclass_Functor = jenv->GetObjectClass( functor );

        jmethodID jmethodID_Functor_call = jenv->GetMethodID( jclass_Functor, "call", "()V" );

        jenv->CallVoidMethod( jclass_Functor, jmethodID_Functor_call );

        Helper::jEnvExceptionCheck( jenv );

        jenv->DeleteLocalRef( jclass_Functor );
    }
    //////////////////////////////////////////////////////////////////////////
}
