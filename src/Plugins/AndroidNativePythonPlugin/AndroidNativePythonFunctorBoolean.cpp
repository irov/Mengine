#include "AndroidNativePythonFunctorBoolean.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Kernel/Error.h"
#include "Kernel/AssertionMemoryPanic.h"

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

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jobject functor = this->getJavaFunctor();

        jclass jclass_FunctorBoolean = jenv->GetObjectClass( functor );

        jmethodID jmethodID_FunctorBoolean_call = jenv->GetMethodID( jclass_FunctorBoolean, "call", "(Z)V" );

        jenv->CallVoidMethod( functor, jmethodID_FunctorBoolean_call, _result );

        Helper::AndroidEnvExceptionCheck(jenv);

        jenv->DeleteLocalRef( jclass_FunctorBoolean );
    }
    //////////////////////////////////////////////////////////////////////////
}
