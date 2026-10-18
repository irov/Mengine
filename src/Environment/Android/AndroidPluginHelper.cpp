#include "AndroidPluginHelper.h"

#include "AndroidHelper.h"
#include "AndroidKernelServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/ConstStringHelper.h"

#include "Config/StdArg.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidPluginMethod( JNIEnv * _jenv, const Char * _plugin, const Char * _method, const Char * _signature, ... )
        {
            ConstString pluginName = Helper::stringizeString( _plugin );
            jobject plugin = ANDROID_KERNEL_SERVICE()
                ->getPlugin( pluginName );

            MENGINE_ASSERTION_FATAL( plugin != nullptr, "android plugin '%s' is not registered", _plugin );

            jclass cls = Mengine_JNI_GetObjectClass( _jenv, plugin );
            jmethodID method = Mengine_JNI_GetMethodID( _jenv, cls, _method, _signature );
            Mengine_JNI_DeleteLocalRef( _jenv, cls );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( method != nullptr, "invalid android plugin '%s' method '%s' signature '%s'", _plugin, _method, _signature );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            Mengine_JNI_CallVoidMethodV( _jenv, plugin, method, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectPluginMethod( JNIEnv * _jenv, const Char * _plugin, const Char * _method, const Char * _signature, ... )
        {
            ConstString pluginName = Helper::stringizeString( _plugin );
            jobject plugin = ANDROID_KERNEL_SERVICE()
                ->getPlugin( pluginName );

            MENGINE_ASSERTION_FATAL( plugin != nullptr, "android plugin '%s' is not registered", _plugin );

            jclass cls = Mengine_JNI_GetObjectClass( _jenv, plugin );
            jmethodID method = Mengine_JNI_GetMethodID( _jenv, cls, _method, _signature );
            Mengine_JNI_DeleteLocalRef( _jenv, cls );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( method != nullptr, "invalid android plugin '%s' method '%s' signature '%s'", _plugin, _method, _signature );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jobject result = Mengine_JNI_CallObjectMethodV( _jenv, plugin, method, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidCallBooleanPluginMethod( JNIEnv * _jenv, const Char * _plugin, const Char * _method, const Char * _signature, ... )
        {
            ConstString pluginName = Helper::stringizeString( _plugin );
            jobject plugin = ANDROID_KERNEL_SERVICE()
                ->getPlugin( pluginName );

            MENGINE_ASSERTION_FATAL( plugin != nullptr, "android plugin '%s' is not registered", _plugin );

            jclass cls = Mengine_JNI_GetObjectClass( _jenv, plugin );
            jmethodID method = Mengine_JNI_GetMethodID( _jenv, cls, _method, _signature );
            Mengine_JNI_DeleteLocalRef( _jenv, cls );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( method != nullptr, "invalid android plugin '%s' method '%s' signature '%s'", _plugin, _method, _signature );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jboolean result = Mengine_JNI_CallBooleanMethodV( _jenv, plugin, method, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
