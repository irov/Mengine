#include "AndroidNativePythonService.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/AndroidKernelServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidDeclaration.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidActivityHelper.h"
#include "Environment/Android/AndroidApplicationHelper.h"

#include "AndroidNativePythonHelper.h"
#include "AndroidNativePythonFunctorVoid.h"
#include "AndroidNativePythonFunctorBoolean.h"
#include "AndroidNativePythonScriptEmbedding.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/Error.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ThreadHelper.h"

#include "Config/StdString.h"
#include "Config/Utility.h"

//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidNativePython_1call )(JNIEnv * env, jclass cls, jstring _plugin, jstring _method, jobjectArray _args)
    {
        Mengine::ConstString plugin = Mengine::Helper::AndroidMakeConstStringFromJString( env, _plugin );
        Mengine::ConstString method = Mengine::Helper::AndroidMakeConstStringFromJString( env, _method );

        if( ANDROID_NATIVEPYTHON_SERVICE()
            ->hasPythonMethod( plugin, method ) == false )
        {
            return;
        }

        jobjectArray new_args = (jobjectArray)env->NewGlobalRef( _args );

        Mengine::Helper::dispatchMainThreadEvent( [plugin, method, new_args]()
        {
            ANDROID_NATIVEPYTHON_SERVICE()
                ->callPythonMethod( plugin, method, new_args );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidNativePython_1addPlugin )(JNIEnv * env, jclass cls, jstring _name, jobject _plugin)
    {
        Mengine::ConstString name = Mengine::Helper::AndroidMakeConstStringFromJString(env, _name);

        jobject new_plugin = env->NewGlobalRef( _plugin );

        ANDROID_NATIVEPYTHON_SERVICE()
            ->addPlugin( name, new_plugin );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidNativePython_1removePlugin )(JNIEnv * env, jclass cls, jstring _name)
    {
        Mengine::ConstString name = Mengine::Helper::AndroidMakeConstStringFromJString(env, _name);

        jobject jplugin = ANDROID_NATIVEPYTHON_SERVICE()
            ->removePlugin( name );

        env->DeleteGlobalRef( jplugin );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AndroidNativePythonService, Mengine::AndroidNativePythonService );
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AndroidNativePythonScriptEmbedding" ), Helper::makeFactorableUnique<AndroidNativePythonScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AndroidNativePythonScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
            ->getKernel();

        m_kernel = kernel;

        m_factoryAndroidNativePythonFunctorVoid = Helper::makeFactoryPool<AndroidNativePythonFunctorVoid, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAndroidNativePythonFunctorBoolean = Helper::makeFactoryPool<AndroidNativePythonFunctorBoolean, 16>( MENGINE_DOCUMENT_FACTORABLE );

        m_callbacksMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );
        m_pluginsMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        m_callbacksMutex = nullptr;
        m_pluginsMutex = nullptr;

        m_callbacks.clear();

        for( auto && [name, jplugin] : m_plugins )
        {
            jenv->DeleteGlobalRef( jplugin );
        }

        m_plugins.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAndroidNativePythonFunctorVoid );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAndroidNativePythonFunctorBoolean );

        m_factoryAndroidNativePythonFunctorVoid = nullptr;
        m_factoryAndroidNativePythonFunctorBoolean = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonService::hasPythonMethod( const ConstString & _plugin, const ConstString & _method ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_callbacksMutex );

        MapAndroidCallbacks::const_iterator it_found = m_callbacks.find( Utility::make_pair( _plugin, _method ) );

        if( it_found == m_callbacks.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::callPythonMethod( const ConstString & _plugin, const ConstString & _method, jobjectArray _args ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        LOGGER_INFO( "android", "call python plugin '%s' method '%s'"
            , _plugin.c_str()
            , _method.c_str()
        );

        MENGINE_THREAD_MUTEX_SCOPE( m_callbacksMutex );

        MapAndroidCallbacks::const_iterator it_found = m_callbacks.find( Utility::make_pair( _plugin, _method ) );

        if( it_found == m_callbacks.end() )
        {
            jenv->DeleteGlobalRef( _args );

            return;
        }

        const VectorAndroidPythonCallbacks & callbacks = it_found->second;

        for( const AndroidPythonCallbackDesc & desc : callbacks )
        {
            uint32_t cb_args_size = m_kernel->tuple_size( desc.args );

            jsize args_size = jenv->GetArrayLength( _args );

            PyObject * py_args = m_kernel->tuple_new( args_size + cb_args_size );

            for( jsize index = 0; index != args_size; ++index )
            {
                jobject obj = jenv->GetObjectArrayElement( _args, index );

                PyObject * py_arg = Helper::androidNativePythonMakePyObject( m_kernel, jenv, obj, MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_ASSERTION_FATAL( py_arg != nullptr, "android plugin '%s' method '%s' invalid arg"
                    , _plugin.c_str()
                    , _method.c_str()
                );

                m_kernel->tuple_setitem( py_args, index, py_arg );

                m_kernel->decref( py_arg );

                jenv->DeleteLocalRef( obj );
            }

            const pybind::args & cb_args = desc.args;

            for( uint32_t index = 0; index != cb_args_size; ++index )
            {
                PyObject * cb_arg = cb_args[index];

                m_kernel->tuple_setitem( py_args, args_size + index, cb_arg );
            }

            const pybind::object & cb = desc.cb;

            pybind::object py_result = cb.call_native( pybind::tuple( m_kernel, py_args, pybind::borrowed ) );

            MENGINE_ASSERTION_FATAL( py_result != nullptr, "android plugin '%s' method '%s' invalid call"
                , _plugin.c_str()
                , _method.c_str()
            );

            m_kernel->decref( py_args );
        }

        jenv->DeleteGlobalRef( _args );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::addPlugin( const ConstString & _name, jobject _jplugin )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( _name, true );

        MENGINE_THREAD_MUTEX_SCOPE( m_pluginsMutex );

        MENGINE_ASSERTION_FATAL( m_plugins.find( _name ) == m_plugins.end(), "invalid add plugin '%s' [double]"
            , _name.c_str()
        );

        m_plugins.emplace( _name, _jplugin );
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidNativePythonService::removePlugin( const ConstString & _name )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( _name, false );

        MENGINE_THREAD_MUTEX_SCOPE( m_pluginsMutex );

        MapAndroidPlugins::iterator it_found = m_plugins.find( _name );

        MENGINE_ASSERTION_FATAL( it_found != m_plugins.end(), "invalid remove plugin '%s' [empty]"
            , _name.c_str()
        );

        jobject jplugin = it_found->second;

        m_plugins.erase( it_found );

        return jplugin;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::object AndroidNativePythonService::addAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb, const pybind::args & _args )
    {
        MapAndroidCallbacks::iterator it_found = m_callbacks.find( Utility::make_pair( _plugin, _method ) );

        if( it_found == m_callbacks.end() )
        {
            VectorAndroidPythonCallbacks new_callbacks;

            it_found = m_callbacks.emplace( Utility::make_pair( Utility::make_pair( _plugin, _method ), new_callbacks ) ).first;
        }

        VectorAndroidPythonCallbacks & callbacks = it_found->second;

        AndroidPythonCallbackDesc desc;
        desc.cb = _cb;
        desc.args = _args;

        callbacks.emplace_back( desc );

        return _cb;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::removeAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb )
    {
        MapAndroidCallbacks::iterator it_found = m_callbacks.find( Utility::make_pair( _plugin, _method ) );

        if( it_found == m_callbacks.end() )
        {
            LOGGER_ERROR("invalid remove android callback plugin '%s' method '%s' not found"
                , _plugin.c_str()
                , _method.c_str()
            );

            return;
        }

        VectorAndroidPythonCallbacks & callbacks = it_found->second;

        VectorAndroidPythonCallbacks::iterator it_callback_found = Algorithm::find_if( callbacks.begin(), callbacks.end(), [_cb](const AndroidPythonCallbackDesc & desc) {
            return desc.cb.ptr() == _cb.ptr();
        } );

        if( it_callback_found == callbacks.end() )
        {
            LOGGER_ERROR("invalid remove android callback plugin '%s' method '%s' not found [cb]"
                , _plugin.c_str()
                , _method.c_str()
            );

            return;
        }

        callbacks.erase( it_callback_found );
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

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "V", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return;
        }

        jenv->CallVoidMethodA( jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }
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

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "Z", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return false;
        }

        jboolean jresult = jenv->CallBooleanMethodA( jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

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

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "I", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return 0;
        }

        jint jresult = jenv->CallIntMethodA( jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

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

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "J", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return 0LL;
        }

        jlong jresult = jenv->CallLongMethodA( jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

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

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "F", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return 0.f;
        }

        jfloat jresult = jenv->CallFloatMethodA( jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

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

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "D", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return 0.0;
        }

        jdouble jresult = jenv->CallDoubleMethodA( jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

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

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "Ljava/lang/String;", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return m_kernel->ret_none();
        }

        jstring jresult = (jstring)jenv->CallObjectMethodA( jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        const Char * jresult_str = jenv->GetStringUTFChars( jresult, nullptr );
        jsize jresult_size = jenv->GetStringLength( jresult );

        PyObject * py_value = m_kernel->string_from_char_size( jresult_str, jresult_size );

        jenv->ReleaseStringUTFChars( jresult, jresult_str );

        jenv->DeleteLocalRef( jresult );

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

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "Ljava/util/Map;", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return m_kernel->ret_none();
        }

        jobject jresult = jenv->CallObjectMethodA( jplugin, jmethodID_method, jargs );

        Helper::AndroidEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        PyObject * py_result = Helper::androidNativePythonMakePyObject( m_kernel, jenv, jresult, MENGINE_DOCUMENT_FACTORABLE );

        return py_result;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::waitSemaphore( const ConstString & _name, const AndroidSemaphoreListenerInterfacePtr & _listener )
    {
        LOGGER_INFO( "android", "wait semaphore '%s'"
            , _name.c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        const Char * name_str = _name.c_str();

        jstring jname = jenv->NewStringUTF( name_str );

        jclass jclass_MengineSemaphoreListener = Mengine_JNI_FindClass( jenv, "org/Mengine/Base/MengineSemaphoreListener" );

        Helper::AndroidEnvExceptionCheck( jenv );

        jmethodID jmethodID_MengineSemaphoreListener_constructor = jenv->GetMethodID( jclass_MengineSemaphoreListener, "<init>", "(Ljava/lang/Object;)V" );

        AndroidSemaphoreListenerInterface * listener_ptr = _listener.get();
        AndroidSemaphoreListenerInterface::intrusive_ptr_add_ref( listener_ptr );
        jobject jpoint_listener = jenv->NewDirectByteBuffer( listener_ptr, sizeof(void *) );

        jobject jfunctor = jenv->NewObject( jclass_MengineSemaphoreListener, jmethodID_MengineSemaphoreListener_constructor, jpoint_listener );

        Helper::AndroidEnvExceptionCheck( jenv );

        Helper::AndroidCallVoidActivityMethod( jenv, "waitSemaphore", "(Ljava/lang/String;Lorg/Mengine/Base/MengineSemaphoreListener;)V", jname, jfunctor );

        jenv->DeleteLocalRef( jname );
        jenv->DeleteLocalRef( jfunctor );
        jenv->DeleteLocalRef( jpoint_listener );
        jenv->DeleteLocalRef( jclass_MengineSemaphoreListener );
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidNativePythonService::getAndroidPlugin( const ConstString & _plugin ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_pluginsMutex );

        MapAndroidPlugins::const_iterator it_found = m_plugins.find( _plugin );

        if( it_found == m_plugins.end() )
        {
            return nullptr;
        }

        jobject jplugin = it_found->second;

        return jplugin;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonService::getAndroidMethod( JNIEnv * _jenv, const ConstString & _plugin, const ConstString & _method, const pybind::args & _args, const Char * _retType, jvalue * const _jargs, jobject * const _jfree, uint32_t * const _freeCount, jobject * const _jplugin, jmethodID * const _jmethodId ) const
    {
        MENGINE_ASSERTION_FATAL( _args.size() <= 32, "android method plugin '%s' method '%s' max args [32 < %u]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.size()
        );

        jobject jplugin = this->getAndroidPlugin( _plugin );

        if( jplugin == nullptr )
        {
            LOGGER_ERROR( "android not register plugin '%s' (call method '%s' args '%s')"
                , _plugin.c_str()
                , _method.c_str()
                , _args.repr().c_str()
            );

            return false;
        }

        jclass plugin_class = _jenv->GetObjectClass( jplugin );

        Char signature[1024 + 1] = {'\0'};

        StdString::strcat( signature, "(" );

        StdString::memset( _jargs, 0, sizeof( jvalue ) * 32 );
        StdString::memset( _jfree, 0, sizeof( jstring ) * 32 );

        uint32_t index_args = 0;
        uint32_t index_free = 0;

        for( const pybind::object & arg : _args )
        {
            if( arg.is_none() == true )
            {
                _jargs[index_args++].l = nullptr;

                StdString::strcat( signature, "L" );
            }
            else if( arg.is_bool() == true )
            {
                jboolean jvalue = (bool)arg.extract();
                _jargs[index_args++].z = jvalue;

                StdString::strcat( signature, "Z" );
            }
            else if( arg.is_integer() == true )
            {
                jint jvalue = (int32_t)arg.extract();
                _jargs[index_args++].i = jvalue;

                StdString::strcat( signature, "I" );
            }
            else if( arg.is_long() == true )
            {
                jlong jvalue = (int64_t)arg.extract();
                _jargs[index_args++].j = jvalue;

                StdString::strcat( signature, "J" );
            }
            else if( arg.is_float() == true )
            {
                jfloat jvalue = (float)arg.extract();
                _jargs[index_args++].f = jvalue;

                StdString::strcat( signature, "F" );
            }
            else if( arg.is_string() == true )
            {
                const Char * value_str = (const Char *)arg.extract();

                jstring jvalue = _jenv->NewStringUTF( value_str );

                _jargs[index_args++].l = jvalue;
                _jfree[index_free++] = jvalue;

                StdString::strcat( signature, "Ljava/lang/String;" );
            }
            else if( arg.is_list() == true )
            {
                pybind::list l = arg.extract();

                pybind::list::size_type s = l.size();

                jclass jclass_ArrayList = _jenv->FindClass( "java/util/ArrayList" );

                jmethodID jmethodID_List_constructor = _jenv->GetMethodID( jclass_ArrayList, "<init>", "(I)V" );

                MENGINE_ASSERTION_FATAL( jmethodID_List_constructor != nullptr, "invalid get android method 'java/lang/ArrayList [<init>] (I)V'" );

                jobject jlist = _jenv->NewObject( jclass_ArrayList, jmethodID_List_constructor, (jsize)s );

                uint32_t index = 0;
                for( const pybind::object & o : l )
                {
                    const Char * o_str = o.extract();

                    jstring jelement = _jenv->NewStringUTF( o_str );

                    jboolean result = Helper::AndroidAddJObjectList( _jenv, jlist, jelement );

                    _jenv->DeleteLocalRef( jelement );

                    ++index;
                }

                _jargs[index_args++].l = jlist;
                _jfree[index_free++] = jlist;

                _jenv->DeleteLocalRef( jclass_ArrayList );

                StdString::strcat( signature, "Ljava/util/List;" );
            }
            else
            {
                _jenv->DeleteLocalRef( plugin_class );

                LOGGER_ERROR( "android plugin '%s' method '%s' unsupported arg [%u] type [%s]"
                    , _plugin.c_str()
                    , _method.c_str()
                    , index_args
                    , arg.repr().c_str()
                );

                return false;
            }
        }

        StdString::strcat( signature, ")" );
        StdString::strcat( signature, _retType );

        jmethodID jmethodId = _jenv->GetMethodID( plugin_class, _method.c_str(), signature );

        if( jmethodId == nullptr )
        {
            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( plugin_class );

            LOGGER_ERROR( "android plugin '%s' not found method '%s' with signature '%s'"
                , _plugin.c_str()
                , _method.c_str()
                , signature
            );

            return false;
        }

        _jenv->DeleteLocalRef( plugin_class );

        *_freeCount = index_free;
        *_jplugin = jplugin;
        *_jmethodId = jmethodId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
