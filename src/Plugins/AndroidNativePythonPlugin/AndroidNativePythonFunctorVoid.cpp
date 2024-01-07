#include "AndroidNativePythonFunctorVoid.h"

#include "Interface/AndroidEnvironmentServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Kernel/Error.h"
#include "Kernel/AssertionMemoryPanic.h"

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

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jobject functor = this->getJavaFunctor();

        jclass jclass_Functor = jenv->GetObjectClass( functor );

        jmethodID jmethodID_Functor_call = jenv->GetMethodID( jclass_Functor, "call", "()V" );

        jenv->CallVoidMethod( functor, jmethodID_Functor_call );

        Helper::jEnvExceptionCheck( jenv );

        jenv->DeleteLocalRef( jclass_Functor );
    }
    //////////////////////////////////////////////////////////////////////////
}
