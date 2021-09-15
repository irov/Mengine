#include "AndroidNativeKernelModule.h"

#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/PluginServiceInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidUtils.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "pybind/pybind.hpp"
#endif

//////////////////////////////////////////////////////////////////////////
Mengine::PluginServiceInterface * g_pluginService = nullptr;
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
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL
    MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeKernel_1hasPlugin )(JNIEnv * env, jclass cls, jstring name)
    {
        if( g_pluginService == nullptr )
        {
            return false;
        }

        const char * name_str = env->GetStringUTFChars( name, 0 );

        bool exist = g_pluginService->hasPlugin(name_str);

        env->ReleaseStringUTFChars( name, name_str );

        return (jboolean)exist;
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
        Mengine::PluginServiceInterface * pluginService = PLUGIN_SERVICE();

        if( pluginService == nullptr )
        {
            return false;
        }

        g_pluginService = pluginService;

#ifdef MENGINE_USE_SCRIPT_SERVICE
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

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
