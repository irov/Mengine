#include "AndroidNativePythonService.h"

#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidDeclaration.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidActivityHelper.h"
#include "Environment/Android/AndroidApplicationHelper.h"
#include "Environment/Android/AndroidKernelServiceInterface.h"

#include "PythonAndroidPluginCallback.h"
#include "AndroidNativePythonHelper.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ThreadHelper.h"

#include "Config/StdString.h"
#include "Config/StdUtility.h"

#ifndef MENGINE_ANDROID_METHOD_MAX_ARGS
#define MENGINE_ANDROID_METHOD_MAX_ARGS 32
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AndroidNativePythonService, Mengine::AndroidNativePythonService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonService::AndroidNativePythonService()
        : m_kernel( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonService::~AndroidNativePythonService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonService::_initializeService()
    {
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
            ->getKernel();

        m_kernel = kernel;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [void]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.repr().c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Mengine_JNI_PushLocalFrame( jenv, MENGINE_ANDROID_METHOD_MAX_ARGS );

        jvalue jargs[MENGINE_ANDROID_METHOD_MAX_ARGS];

        jobject jplugin;
        jmethodID jmethodID_method;

        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "V", jargs, &jplugin, &jmethodID_method ) == false )
        {
            Mengine_JNI_PopLocalFrame( jenv, nullptr );

            return;
        }

        Mengine_JNI_CallVoidMethodA( jenv, jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        Mengine_JNI_PopLocalFrame( jenv, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonService::androidBooleanMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [boolean]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.repr().c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Mengine_JNI_PushLocalFrame( jenv, MENGINE_ANDROID_METHOD_MAX_ARGS );

        jvalue jargs[MENGINE_ANDROID_METHOD_MAX_ARGS];

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "Z", jargs, &jplugin, &jmethodID_method ) == false )
        {
            Mengine_JNI_PopLocalFrame( jenv, nullptr );

            return false;
        }

        jboolean jresult = Mengine_JNI_CallBooleanMethodA( jenv, jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        Mengine_JNI_PopLocalFrame( jenv, nullptr );

        return (bool)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidNativePythonService::androidIntegerMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [int]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.repr().c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Mengine_JNI_PushLocalFrame( jenv, MENGINE_ANDROID_METHOD_MAX_ARGS );

        jvalue jargs[MENGINE_ANDROID_METHOD_MAX_ARGS];

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "I", jargs, &jplugin, &jmethodID_method ) == false )
        {
            Mengine_JNI_PopLocalFrame( jenv, nullptr );

            return 0;
        }

        jint jresult = Mengine_JNI_CallIntMethodA( jenv, jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        Mengine_JNI_PopLocalFrame( jenv, nullptr );

        return (int32_t)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidNativePythonService::androidLongMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [long]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.repr().c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Mengine_JNI_PushLocalFrame( jenv, MENGINE_ANDROID_METHOD_MAX_ARGS );

        jvalue jargs[MENGINE_ANDROID_METHOD_MAX_ARGS];

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "J", jargs, &jplugin, &jmethodID_method ) == false )
        {
            Mengine_JNI_PopLocalFrame( jenv, nullptr );

            return 0LL;
        }

        jlong jresult = Mengine_JNI_CallLongMethodA( jenv, jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        Mengine_JNI_PopLocalFrame( jenv, nullptr );

        return (int64_t)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    float AndroidNativePythonService::androidFloatMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [float]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.repr().c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Mengine_JNI_PushLocalFrame( jenv, MENGINE_ANDROID_METHOD_MAX_ARGS );

        jvalue jargs[MENGINE_ANDROID_METHOD_MAX_ARGS];

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "F", jargs, &jplugin, &jmethodID_method ) == false )
        {
            Mengine_JNI_PopLocalFrame( jenv, nullptr );

            return 0.f;
        }

        jfloat jresult = Mengine_JNI_CallFloatMethodA( jenv, jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        Mengine_JNI_PopLocalFrame( jenv, nullptr );

        return (float)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    double AndroidNativePythonService::androidDoubleMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [double]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.repr().c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Mengine_JNI_PushLocalFrame( jenv, MENGINE_ANDROID_METHOD_MAX_ARGS );

        jvalue jargs[MENGINE_ANDROID_METHOD_MAX_ARGS];

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "D", jargs, &jplugin, &jmethodID_method ) == false )
        {
            Mengine_JNI_PopLocalFrame( jenv, nullptr );

            return 0.0;
        }

        jdouble jresult = Mengine_JNI_CallDoubleMethodA( jenv, jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        Mengine_JNI_PopLocalFrame( jenv, nullptr );

        return (double)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * AndroidNativePythonService::androidStringMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [string]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.repr().c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Mengine_JNI_PushLocalFrame( jenv, MENGINE_ANDROID_METHOD_MAX_ARGS );

        jvalue jargs[MENGINE_ANDROID_METHOD_MAX_ARGS];

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "Ljava/lang/String;", jargs, &jplugin, &jmethodID_method ) == false )
        {
            Mengine_JNI_PopLocalFrame( jenv, nullptr );

            return m_kernel->ret_none();
        }

        jstring jstring_result = (jstring)Mengine_JNI_CallObjectMethodA( jenv, jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        const Char * jresult_str = Mengine_JNI_GetStringUTFChars( jenv, jstring_result, nullptr );
        jsize jresult_size = Mengine_JNI_GetStringLength( jenv, jstring_result );

        PyObject * py_value = m_kernel->string_from_char_size( jresult_str, jresult_size );

        Mengine_JNI_ReleaseStringUTFChars( jenv, jstring_result, jresult_str );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_result );

        Mengine_JNI_PopLocalFrame( jenv, nullptr );

        return py_value;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * AndroidNativePythonService::androidObjectMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [config]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.repr().c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Mengine_JNI_PushLocalFrame( jenv, MENGINE_ANDROID_METHOD_MAX_ARGS );

        jvalue jargs[MENGINE_ANDROID_METHOD_MAX_ARGS];

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "Ljava/lang/Object;", jargs, &jplugin, &jmethodID_method ) == false )
        {
            Mengine_JNI_PopLocalFrame( jenv, nullptr );

            return m_kernel->ret_none();
        }

        jobject jresult = Mengine_JNI_CallObjectMethodA( jenv, jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        PyObject * py_result = Helper::androidNativePythonMakePyObject( m_kernel, jenv, jresult, MENGINE_DOCUMENT_FACTORABLE );

        Mengine_JNI_DeleteLocalRef( jenv, jresult );

        Mengine_JNI_PopLocalFrame( jenv, nullptr );

        return py_result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * AndroidNativePythonService::androidJSONObjectMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [config]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.repr().c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Mengine_JNI_PushLocalFrame( jenv, MENGINE_ANDROID_METHOD_MAX_ARGS );

        jvalue jargs[MENGINE_ANDROID_METHOD_MAX_ARGS];

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "Lorg/json/JSONObject;", jargs, &jplugin, &jmethodID_method ) == false )
        {
            Mengine_JNI_PopLocalFrame( jenv, nullptr );

            return m_kernel->ret_none();
        }

        jobject jresult = Mengine_JNI_CallObjectMethodA( jenv, jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        PyObject * py_result = Helper::androidNativePythonMakePyObject( m_kernel, jenv, jresult, MENGINE_DOCUMENT_FACTORABLE );

        Mengine_JNI_DeleteLocalRef( jenv, jresult );

        Mengine_JNI_PopLocalFrame( jenv, nullptr );

        return py_result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonService::getAndroidMethod( JNIEnv * _jenv, const ConstString & _plugin, const ConstString & _method, const pybind::args & _args, const Char * _retType, jvalue * const _jargs, jobject * const _jplugin, jmethodID * const _jmethodId ) const
    {
        MENGINE_ASSERTION_FATAL( _args.size() <= 32, "android method plugin '%s' method '%s' max args [32 < %zu]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.size()
        );

        jobject jplugin = ANDROID_KERNEL_SERVICE()
            ->getPlugin( _plugin );

        if( jplugin == nullptr )
        {
            LOGGER_ERROR( "android not register plugin '%s' (call method '%s' args '%s')"
                , _plugin.c_str()
                , _method.c_str()
                , _args.repr().c_str()
            );

            return false;
        }

        Char signature[1024 + 1] = {'\0'};

        StdString::strcat_safe( signature, "(", 1024 );

        StdString::memset( _jargs, 0, sizeof( jvalue ) * MENGINE_ANDROID_METHOD_MAX_ARGS );

        uint32_t index_args = 0;
        uint32_t index_free = 0;

        for( const pybind::object & arg : _args )
        {
            if( arg.is_none() == true )
            {
                _jargs[index_args++].l = nullptr;

                StdString::strcat_safe( signature, "L", 1024 );
            }
            else if( arg.is_bool() == true )
            {
                jboolean jvalue = (bool)arg.extract();
                _jargs[index_args++].z = jvalue;

                StdString::strcat_safe( signature, "Z", 1024 );
            }
            else if( arg.is_integer() == true )
            {
                jint jvalue = (int32_t)arg.extract();
                _jargs[index_args++].i = jvalue;

                StdString::strcat_safe( signature, "I", 1024 );
            }
            else if( arg.is_long() == true )
            {
                jlong jvalue = (int64_t)arg.extract();
                _jargs[index_args++].j = jvalue;

                StdString::strcat_safe( signature, "J", 1024 );
            }
            else if( arg.is_float() == true )
            {
                jfloat jvalue = (float)arg.extract();
                _jargs[index_args++].f = jvalue;

                StdString::strcat_safe( signature, "F", 1024 );
            }
            else if( arg.is_string() == true )
            {
                const Char * value_str = (const Char *)arg.extract();

                jstring jvalue = Mengine_JNI_NewStringUTF( _jenv, value_str );

                _jargs[index_args++].l = jvalue;

                StdString::strcat_safe( signature, "Ljava/lang/String;", 1024 );
            }
            else if( arg.is_list() == true )
            {
                pybind::list l = arg.extract();

                jobject jlist = Helper::androidNativePythonListMakeJavaObject( _jenv, l );

                _jargs[index_args++].l = jlist;

                StdString::strcat_safe( signature, "Ljava/util/List;", 1024 );
            }
            else if( arg.is_dict() == true )
            {
                pybind::dict d = arg.extract();

                jobject jmap = Helper::androidNativePythonDictMakeJavaObject( _jenv, d );

                _jargs[index_args++].l = jmap;

                StdString::strcat_safe( signature, "Ljava/util/Map;", 1024 );
            }
            else
            {
                LOGGER_ERROR( "android plugin '%s' method '%s' unsupported arg [%u] type [%s]"
                    , _plugin.c_str()
                    , _method.c_str()
                    , index_args
                    , arg.repr().c_str()
                );

                return false;
            }
        }

        StdString::strcat_safe( signature, ")", 1024 );
        StdString::strcat_safe( signature, _retType, 1024 );

        jclass jclass_PluginClass = Mengine_JNI_GetObjectClass( _jenv, jplugin );

        const Char * method_str = _method.c_str();

        jmethodID jmethodId = Mengine_JNI_GetMethodID( _jenv, jclass_PluginClass, method_str, signature );

        if( jmethodId == nullptr )
        {
            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_PluginClass );

            LOGGER_FATAL( "android plugin '%s' not found method '%s' with signature '%s'"
                , _plugin.c_str()
                , _method.c_str()
                , signature
            );

            return false;
        }

        Mengine_JNI_DeleteLocalRef( _jenv, jclass_PluginClass );

        *_jplugin = jplugin;
        *_jmethodId = jmethodId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
