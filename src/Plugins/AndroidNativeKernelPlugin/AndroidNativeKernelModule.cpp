#include "AndroidNativeKernelModule.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"

#include "Environment/Android/AndroidUtils.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "pybind/pybind.hpp"
#endif

#include <jni.h>
#include <vector>

//////////////////////////////////////////////////////////////////////////
static jclass mActivityClass;
static jmethodID jmethodID_getAndroidId;
//////////////////////////////////////////////////////////////////////////
extern "C" {
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeKernel_1setupKernelJNI )(JNIEnv * env, jclass cls)
    {
        mActivityClass = (jclass)(env->NewGlobalRef( cls ));

        jmethodID_getAndroidId = env->GetStaticMethodID( mActivityClass, "kernelGetAndroidId", "()Ljava/lang/String;" );
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeKernelModule::AndroidNativeKernelModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeKernelModule::~AndroidNativeKernelModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeKernelModule::_initializeModule()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::def_functor( kernel, "androidKernelGetAndroidId", this, &AndroidNativeKernelModule::getAndroidId );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeKernelModule::_finalizeModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeKernelModule::_update( bool _focus )
    {
        MENGINE_UNUSED(_focus);
    }
    //////////////////////////////////////////////////////////////////////////
    String AndroidNativeKernelModule::getAndroidId() const
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jstring jReturnValue = (jstring)env->CallStaticObjectMethod( mActivityClass, jmethodID_getAndroidId );

        const Char * jStringValue = env->GetStringUTFChars( jReturnValue, 0 );

        String stringValue = jStringValue;

        env->ReleaseStringUTFChars( jReturnValue, jStringValue );

        return stringValue;
    }
    //////////////////////////////////////////////////////////////////////////
}
