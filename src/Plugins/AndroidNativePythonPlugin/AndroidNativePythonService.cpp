#include "AndroidNativePythonService.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/AndroidEnvironmentServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/Error.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
static Mengine::AndroidNativePythonService * s_androidNativePythonService = nullptr;
//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1call )(JNIEnv * env, jclass cls, jstring _plugin, jstring _method, int _id, jobjectArray _args)
    {
        Mengine::String plugin = Mengine::Helper::makeStringFromJString( env, _plugin );
        Mengine::String method = Mengine::Helper::makeStringFromJString( env, _method );

        if( s_androidNativePythonService == nullptr )
        {
            __android_log_print(ANDROID_LOG_ERROR, "Mengine", "invalid android call plugin '%s' method '%s'"
                , plugin.c_str()
                , method.c_str()
            );

            return;
        }

        jobjectArray new_args = (jobjectArray)env->NewGlobalRef( _args );

        s_androidNativePythonService->addCommand([plugin, method, _id, new_args]( const Mengine::AndroidNativePythonEventHandlerInterfacePtr & _handler )
        {
            _handler->pythonMethod( plugin, method, _id, new_args );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1addPlugin )(JNIEnv * env, jclass cls, jstring _name, jobject _plugin)
    {
        Mengine::String name = Mengine::Helper::makeStringFromJString( env, _name );

        if( s_androidNativePythonService == nullptr )
        {
            __android_log_print(ANDROID_LOG_ERROR, "Mengine", "invalid android add plugin '%s'"
                , name.c_str()
            );

            return;
        }

        jobject new_plugin = env->NewGlobalRef( _plugin );

        s_androidNativePythonService->addCommand([name, new_plugin]( const Mengine::AndroidNativePythonEventHandlerInterfacePtr & _handler )
        {
            _handler->addPlugin( name, new_plugin );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1activateSemaphore )(JNIEnv * env, jclass cls, jstring _name)
    {
        Mengine::String name = Mengine::Helper::makeStringFromJString( env, _name );

        if( s_androidNativePythonService == nullptr )
        {
            __android_log_print(ANDROID_LOG_ERROR, "Mengine", "invalid android activate semaphore '%s'"
                , name.c_str()
            );

            return;
        }

        s_androidNativePythonService->addCommand([name]( const Mengine::AndroidNativePythonEventHandlerInterfacePtr & _handler )
        {
            _handler->activateSemaphore( name );
        } );
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
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
            ->getKernel();

        pybind::def_functor( kernel, "setAndroidCallback", this, &AndroidNativePythonService::setAndroidCallback );
        pybind::def_functor_args( kernel, "androidMethod", this, &AndroidNativePythonService::androidMethod );
        pybind::def_functor_args( kernel, "androidBooleanMethod", this, &AndroidNativePythonService::androidBooleanMethod );
        pybind::def_functor_args( kernel, "androidIntegerMethod", this, &AndroidNativePythonService::androidIntegerMethod );
        pybind::def_functor_args( kernel, "androidLongMethod", this, &AndroidNativePythonService::androidLongMethod );
        pybind::def_functor_args( kernel, "androidFloatMethod", this, &AndroidNativePythonService::androidFloatMethod );
        pybind::def_functor_args( kernel, "androidDoubleMethod", this, &AndroidNativePythonService::androidDoubleMethod );
        pybind::def_functor_args( kernel, "androidStringMethod", this, &AndroidNativePythonService::androidStringMethod );
        pybind::def_functor_args( kernel, "androidConfigMethod", this, &AndroidNativePythonService::androidConfigMethod );

        pybind::def_functor_args( kernel, "waitAndroidSemaphore", this, &AndroidNativePythonService::waitAndroidSemaphore );

        m_kernel = kernel;

        m_eventation = Helper::makeFactorableUnique<PythonEventation>( MENGINE_DOCUMENT_FACTORABLE );

        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_eventation->initialize( mutex ) == false )
        {
            return false;
        }

        s_androidNativePythonService = this;

        m_eventation->setEventHandler( AndroidNativePythonEventHandlerInterfacePtr::from( this ) );

        ANDROID_ENVIRONMENT_SERVICE()
            ->addAndroidEventation( m_eventation );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return false;
        }

        jmethodID jmethodID_initializePlugins = ANDROID_ENVIRONMENT_SERVICE()
                ->getActivityMethodID(jenv, "pythonInitializePlugins", "()V");

        ANDROID_ENVIRONMENT_SERVICE()
                ->callVoidActivityMethod(jenv, jmethodID_initializePlugins);

        ANDROID_ENVIRONMENT_SERVICE()
            ->invokeAndroidEventations();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::_finalizeService()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return;
        }

        jmethodID jmethodID_pythonFinalizePlugins = ANDROID_ENVIRONMENT_SERVICE()
                ->getActivityMethodID(jenv, "pythonFinalizePlugins", "()V");

        ANDROID_ENVIRONMENT_SERVICE()
                ->callVoidActivityMethod(jenv, jmethodID_pythonFinalizePlugins);

        ANDROID_ENVIRONMENT_SERVICE()
            ->invokeAndroidEventations();

        s_androidNativePythonService = nullptr;

        m_semaphoreListeners.clear();
        m_callbacks.clear();

        for( auto && [name, jplugin] : m_plugins )
        {
            jenv->DeleteGlobalRef( jplugin );
        }

        m_plugins.clear();

        ANDROID_ENVIRONMENT_SERVICE()
            ->removeAndroidEventation( m_eventation );

        m_eventation->finalize();
        m_eventation = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::addCommand( const LambdaPythonEventHandler & _command )
    {
        m_eventation->addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * AndroidNativePythonService::makePythonAttribute( JNIEnv * _jenv, jobject _obj )
    {
        PyObject * py_value = nullptr;

        jclass jclass_Boolean = _jenv->FindClass( "java/lang/Boolean" );
        jclass jclass_Character = _jenv->FindClass( "java/lang/Character" );
        jclass jclass_Integer = _jenv->FindClass( "java/lang/Integer" );
        jclass jclass_Long = _jenv->FindClass( "java/lang/Long" );
        jclass jclass_Float = _jenv->FindClass( "java/lang/Float" );
        jclass jclass_Double = _jenv->FindClass( "java/lang/Double" );
        jclass jclass_String = _jenv->FindClass( "java/lang/String" );
        jclass jclass_ArrayList = _jenv->FindClass( "java/util/ArrayList" );

        if( _obj == nullptr )
        {
            py_value = m_kernel->ret_none();
        }
        else if( _jenv->IsInstanceOf(_obj, jclass_Boolean ) == JNI_TRUE )
        {
            jmethodID methodValue = _jenv->GetMethodID( jclass_Boolean, "booleanValue", "()Z" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Boolean [booleanValue] ()C'" );

            jboolean value = _jenv->CallBooleanMethod( _obj, methodValue );

            Helper::jEnvExceptionCheck( _jenv );

            py_value = m_kernel->ret_bool( value );
        }
        else if( _jenv->IsInstanceOf(_obj, jclass_Character ) == JNI_TRUE )
        {
            jmethodID methodValue = _jenv->GetMethodID( jclass_Character, "charValue", "()C" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Character [charValue] ()C'" );

            jchar value = _jenv->CallCharMethod( _obj, methodValue );

            Helper::jEnvExceptionCheck( _jenv );

            Char value_str[2] = {(Char)value, '\0'};
            py_value = m_kernel->string_from_char_size( value_str, 1 );
        }
        else if( _jenv->IsInstanceOf(_obj, jclass_Integer ) == JNI_TRUE )
        {
            jmethodID methodValue = _jenv->GetMethodID( jclass_Integer, "intValue", "()I" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Integer [intValue] ()I'" );

            jint value = _jenv->CallIntMethod( _obj, methodValue );

            Helper::jEnvExceptionCheck( _jenv );

            py_value = m_kernel->ptr_int32( value );
        }
        else if( _jenv->IsInstanceOf( _obj, jclass_Long ) == JNI_TRUE )
        {
            jmethodID methodValue = _jenv->GetMethodID( jclass_Long, "longValue", "()J" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Long [longValue] ()J'" );

            jlong value = _jenv->CallLongMethod(_obj, methodValue );

            Helper::jEnvExceptionCheck( _jenv );

            py_value = m_kernel->ptr_int64( value );
        }
        else if( _jenv->IsInstanceOf( _obj, jclass_Float ) == JNI_TRUE )
        {
            jmethodID methodValue = _jenv->GetMethodID( jclass_Float, "floatValue", "()F" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Float [floatValue] ()F'" );

            jfloat value = _jenv->CallFloatMethod( _obj, methodValue );

            Helper::jEnvExceptionCheck( _jenv );

            py_value = m_kernel->ptr_float( value );
        }
        else if( _jenv->IsInstanceOf( _obj, jclass_Double ) == JNI_TRUE )
        {
            jmethodID methodValue = _jenv->GetMethodID( jclass_Double, "doubleValue", "()D" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Double [doubleValue] ()D'" );

            jfloat value = _jenv->CallDoubleMethod( _obj, methodValue );

            Helper::jEnvExceptionCheck( _jenv );

            py_value = m_kernel->ptr_float( value );
        }
        else if( _jenv->IsInstanceOf(_obj, jclass_String ) == JNI_TRUE )
        {
            const Char * obj_str = _jenv->GetStringUTFChars( (jstring)_obj, nullptr );

            py_value = m_kernel->string_from_char( obj_str );

            _jenv->ReleaseStringUTFChars( (jstring)_obj, obj_str );
        }
        else if( _jenv->IsInstanceOf( _obj, jclass_ArrayList ) == JNI_TRUE )
        {
            jfieldID fieldElementData = _jenv->GetFieldID( jclass_ArrayList, "elementData", "[Ljava/lang/Object;" );

            MENGINE_ASSERTION_FATAL( fieldElementData != nullptr );

            jobjectArray list_elementData = (jobjectArray)_jenv->GetObjectField( _obj, fieldElementData );

            jsize list_size = _jenv->GetArrayLength( list_elementData );

            PyObject * py_list = m_kernel->list_new( list_size );

            for( jsize index = 0; index != list_size; ++index )
            {
                jobject list_obj = _jenv->GetObjectArrayElement( list_elementData, index );

                PyObject * py_obj = this->makePythonAttribute( _jenv, list_obj );

                MENGINE_ASSERTION_FATAL( py_obj != nullptr );

                m_kernel->list_setitem( py_list, index, py_obj );

                m_kernel->decref( py_obj );

                _jenv->DeleteLocalRef( list_obj );
            }

            _jenv->DeleteLocalRef( list_elementData );

            py_value = py_list;
        }
        else
        {
            jclass cls_obj = _jenv->GetObjectClass( _obj );

            jmethodID jmethodID_getClass = _jenv->GetMethodID( cls_obj, "getClass", "()Ljava/lang/Class;" );

            MENGINE_ASSERTION_FATAL( jmethodID_getClass != nullptr, "invalid get android method 'getClass()Ljava/lang/Class;'" );

            jobject obj_class = _jenv->CallObjectMethod( _obj, jmethodID_getClass );

            Helper::jEnvExceptionCheck( _jenv );

            jclass cls_class = _jenv->GetObjectClass( obj_class );

            jmethodID jmethodID_getName = _jenv->GetMethodID( cls_class, "getName", "()Ljava/lang/String;" );

            MENGINE_ASSERTION_FATAL( jmethodID_getName != nullptr, "invalid get android method 'getName()Ljava/lang/String;'" );

            jstring obj_class_name = (jstring)_jenv->CallObjectMethod( obj_class, jmethodID_getName );

            Helper::jEnvExceptionCheck( _jenv );

            const Char * obj_class_name_str = _jenv->GetStringUTFChars( obj_class_name, nullptr );

            LOGGER_ERROR( "unsupported java argument type '%s'"
                , obj_class_name_str
            );

            _jenv->ReleaseStringUTFChars( obj_class_name, obj_class_name_str );
            _jenv->DeleteLocalRef( obj_class_name );

            _jenv->DeleteLocalRef( obj_class );
            _jenv->DeleteLocalRef( cls_class );
            _jenv->DeleteLocalRef( cls_obj );
        }

        _jenv->DeleteLocalRef( jclass_Boolean );
        _jenv->DeleteLocalRef( jclass_Character );
        _jenv->DeleteLocalRef( jclass_Integer );
        _jenv->DeleteLocalRef( jclass_Long );
        _jenv->DeleteLocalRef( jclass_Float );
        _jenv->DeleteLocalRef( jclass_Double );
        _jenv->DeleteLocalRef( jclass_String );
        _jenv->DeleteLocalRef( jclass_ArrayList );

        return py_value;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::pythonMethod( const String & _plugin, const String & _method, int32_t _id, jobjectArray _args )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return;
        }

        ConstString plugin_c = Helper::stringizeString( _plugin );
        ConstString method_c = Helper::stringizeString( _method );

        LOGGER_INFO( "android", "call python plugin '%s' method '%s' [%s]"
            , _plugin.c_str()
            , _method.c_str()
            , (m_callbacks.find( Helper::makePair( plugin_c, method_c ) ) != m_callbacks.end() ? "Found" : "NOT-FOUND")
        );

        MapAndroidCallbacks::const_iterator it_found = m_callbacks.find( Helper::makePair( plugin_c, method_c ) );

        if( it_found == m_callbacks.end() )
        {
            jenv->DeleteGlobalRef( _args );

            return;
        }

        jsize args_size = jenv->GetArrayLength( _args );

        PyObject * py_args = m_kernel->tuple_new( args_size );

        for( jsize index = 0; index != args_size; ++index )
        {
            jobject obj = jenv->GetObjectArrayElement( _args, index );

            PyObject * py_arg = this->makePythonAttribute( jenv, obj );

            MENGINE_ASSERTION_FATAL( py_arg != nullptr, "android plugin '%s' method '%s' id '%d' invalid arg"
                , _plugin.c_str()
                , _method.c_str()
                , _id
            );

            m_kernel->tuple_setitem( py_args, index, py_arg );

            m_kernel->decref( py_arg );

            jenv->DeleteLocalRef( obj );
        }

        jenv->DeleteGlobalRef( _args );

        const pybind::object & cb = it_found->second;

        pybind::object py_result = cb.call_native( pybind::tuple( m_kernel, py_args, pybind::borrowed ) );

        MENGINE_ASSERTION_FATAL( py_result != nullptr, "android plugin '%s' method '%s' id '%d' invalid call"
            , _plugin.c_str()
            , _method.c_str()
            , _id
        );

        MENGINE_ASSERTION_FATAL( !(py_result.is_none() == true && _id != 0), "android plugin '%s' method '%s' return 'None' but have callback '%d'"
            , _plugin.c_str()
            , _method.c_str()
            , _id
        );

        MENGINE_ASSERTION_FATAL( !(py_result.is_none() == false && _id == 0), "android plugin '%s' method '%s' return '%s' but not setup callback"
            , _plugin.c_str()
            , _method.c_str()
            , py_result.str().c_str()
        );

        m_kernel->decref( py_args );

        if( _id == 0 )
        {
            return;
        }

        this->androidResponse( jenv, _id, py_result );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::addPlugin( const String & _name, jobject _plugin )
    {
        MENGINE_ASSERTION_FATAL( m_plugins.find( Helper::stringizeString( _name ) ) == m_plugins.end(), "invalid add plugin '%s' [double]"
            , _name.c_str()
        );

        SCRIPT_SERVICE()
            ->setAvailablePlugin( _name.c_str(), true );

        m_plugins.emplace( Helper::stringizeString( _name ), _plugin );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::setAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb )
    {
        MENGINE_ASSERTION_FATAL( m_callbacks.find( Helper::makePair( _plugin, _method ) ) == m_callbacks.end(), "invalid add plugin '%s' callback '%s' [double]"
            , _plugin.c_str()
            , _method.c_str()
        );

        m_callbacks.emplace( Helper::makePair( _plugin, _method ), _cb );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonService::androidResponse( JNIEnv * _jenv, int32_t _id, const pybind::object & _result ) const
    {
        jobject jresult;

        if( _result.is_none() == true )
        {
            jclass jclass_Object = _jenv->FindClass( "java/lang/Object" );

            jmethodID constructor = _jenv->GetMethodID( jclass_Object, "<init>", "()V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] ()V'" );

            jresult = _jenv->NewObject( jclass_Object, constructor );

            _jenv->DeleteLocalRef( jclass_Object );
        }
        else if( _result.is_bool() == true )
        {
            bool value = _result.extract();

            jresult = Helper::makeJObjectBoolean( _jenv, value );
        }
        else if( _result.is_integer() == true )
        {
            int32_t value = _result.extract();

            jresult = Helper::makeJObjectInteger( _jenv, value );
        }
        else if( _result.is_long() == true )
        {
            int64_t value = _result.extract();

            jresult = Helper::makeJObjectLong( _jenv, value );
        }
        else if( _result.is_float() == true )
        {
            double value = _result.extract();

            jresult = Helper::makeJObjectDouble( _jenv, value );
        }
        else if( _result.is_string() == true )
        {
            const Char * value_str = (const Char *)_result.extract();

            jresult = Helper::makeJObjectString( _jenv, value_str );
        }
        else
        {
            LOGGER_ERROR( "android response call '%d' unsupported result type [%s]"
                , _id
                , _result.repr_type().c_str()
            );

            return false;
        }

        jmethodID jmethodID_pythonCallResponse = ANDROID_ENVIRONMENT_SERVICE()
                ->getActivityMethodID(_jenv, "pythonCallResponse", "(ILjava/lang/Object;)V");

        ANDROID_ENVIRONMENT_SERVICE()
                ->callVoidActivityMethod(_jenv, jmethodID_pythonCallResponse, _id, jresult);

        _jenv->DeleteLocalRef( jresult );

        ANDROID_ENVIRONMENT_SERVICE()
            ->invokeAndroidEventations();

        return true;
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

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return;
        }

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

        Helper::jEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        ANDROID_ENVIRONMENT_SERVICE()
            ->invokeAndroidEventations();
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

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return false;
        }

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

        Helper::jEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        ANDROID_ENVIRONMENT_SERVICE()
            ->invokeAndroidEventations();

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

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return 0;
        }

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

        Helper::jEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        ANDROID_ENVIRONMENT_SERVICE()
            ->invokeAndroidEventations();

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

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return 0ULL;
        }

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "J", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return 0ULL;
        }

        jlong jresult = jenv->CallLongMethodA( jplugin, jmethodID_method, jargs );

        Helper::jEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        ANDROID_ENVIRONMENT_SERVICE()
            ->invokeAndroidEventations();

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

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return 0.f;
        }

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

        Helper::jEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        ANDROID_ENVIRONMENT_SERVICE()
            ->invokeAndroidEventations();

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

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return 0.0;
        }

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

        Helper::jEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        ANDROID_ENVIRONMENT_SERVICE()
                ->invokeAndroidEventations();

        return (double)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    String AndroidNativePythonService::androidStringMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [string]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.repr().c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return String();
        }

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "Ljava/lang/String;", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return String();
        }

        jstring jresult = (jstring)jenv->CallObjectMethodA( jplugin, jmethodID_method, jargs );

        Helper::jEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        Mengine::String result = Helper::makeStringFromJString( jenv, jresult );

        jenv->DeleteLocalRef( jresult );

        ANDROID_ENVIRONMENT_SERVICE()
            ->invokeAndroidEventations();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * AndroidNativePythonService::androidConfigMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [config]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.repr().c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return m_kernel->ret_none();
        }

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

        Helper::jEnvExceptionCheck( jenv );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        PyObject * pyresult = m_kernel->dict_new();

        jclass jclass_Map = jenv->FindClass( "java/util/Map" );
        jclass jclass_Set = jenv->FindClass( "java/util/Set" );
        jclass jclass_Iterator = jenv->FindClass( "java/util/Iterator" );
        jclass jclass_MapEntry = jenv->FindClass( "java/util/Map$Entry" );

        jmethodID jmethodID_Map_entrySet = jenv->GetMethodID( jclass_Map, "entrySet", "()Ljava/util/Set;" );
        jmethodID jmethodID_Set_iterator = jenv->GetMethodID( jclass_Set, "iterator", "()Ljava/util/Iterator;" );
        jmethodID jmethodID_Iterator_hasNext = jenv->GetMethodID( jclass_Iterator, "hasNext", "()Z" );
        jmethodID jmethodID_Iterator_next = jenv->GetMethodID( jclass_Iterator, "next", "()Ljava/lang/Object;" );
        jmethodID jmethodID_MapEntry_getKey = jenv->GetMethodID( jclass_MapEntry, "getKey", "()Ljava/lang/Object;" );
        jmethodID jmethodID_MapEntry_getValue = jenv->GetMethodID( jclass_MapEntry, "getValue", "()Ljava/lang/Object;" );

        jobject jset = jenv->CallObjectMethod( jresult, jmethodID_Map_entrySet );
        jobject jset_iterator = jenv->CallObjectMethod( jset, jmethodID_Set_iterator );

        jboolean hasNext = jenv->CallBooleanMethod( jset_iterator, jmethodID_Iterator_hasNext );

        while( hasNext == JNI_TRUE )
        {
            jobject jentry = jenv->CallObjectMethod(jset_iterator, jmethodID_Iterator_next );

            jstring jkey = (jstring)jenv->CallObjectMethod( jentry, jmethodID_MapEntry_getKey );
            jstring jvalue = (jstring)jenv->CallObjectMethod( jentry, jmethodID_MapEntry_getValue );

            const Char * key_str = jenv->GetStringUTFChars( jkey, 0 );
            const Char * value_str = jenv->GetStringUTFChars( jvalue, 0 );

            PyObject * pyvalue_str = m_kernel->string_from_char( value_str );
            m_kernel->dict_setstring( pyresult, key_str, pyvalue_str );
            m_kernel->decref( pyvalue_str );

            jenv->ReleaseStringUTFChars( jkey, key_str );
            jenv->ReleaseStringUTFChars( jvalue, value_str );

            jenv->DeleteLocalRef( jkey );
            jenv->DeleteLocalRef( jvalue );
            jenv->DeleteLocalRef( jentry );

            hasNext = jenv->CallBooleanMethod( jset_iterator, jmethodID_Iterator_hasNext );
        }

        jenv->DeleteLocalRef( jset_iterator );
        jenv->DeleteLocalRef( jset );
        jenv->DeleteLocalRef( jresult );

        jenv->DeleteLocalRef( jclass_Map );
        jenv->DeleteLocalRef( jclass_Set );
        jenv->DeleteLocalRef( jclass_Iterator );
        jenv->DeleteLocalRef( jclass_MapEntry );

        Helper::jEnvExceptionCheck( jenv );

        ANDROID_ENVIRONMENT_SERVICE()
            ->invokeAndroidEventations();

        return pyresult;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonService::getAndroidMethod( JNIEnv * _jenv, const ConstString & _plugin, const ConstString & _method, const pybind::args & _args, const Char * _retType, jvalue * const _jargs, jobject * const _jfree, uint32_t * const _freeCount, jobject * const _jplugin, jmethodID * const _jmethodId ) const
    {
        MENGINE_ASSERTION_FATAL( _args.size() <= 32, "android method plugin '%s' method '%s' max args [32 < %u]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.size()
        );

        MapAndroidPlugins::const_iterator it_found = m_plugins.find( _plugin );

        if( it_found == m_plugins.end() )
        {
            LOGGER_ERROR( "android not register plugin '%s' (call method '%s' args '%s')"
                , _plugin.c_str()
                , _method.c_str()
                , _args.repr().c_str()
            );

            return false;
        }

        jobject jplugin = it_found->second;

        jclass plugin_class = _jenv->GetObjectClass( jplugin );

        if( jplugin == nullptr )
        {
            Helper::jEnvExceptionCheck( _jenv );

            LOGGER_ERROR( "android not found java plugin '%s' (call method '%s' args '%s')"
                , _plugin.c_str()
                , _method.c_str()
                , _args.repr().c_str()
            );

            return false;
        }

        Char signature[1024] = {'\0'};

        MENGINE_STRCAT( signature, "(" );

        MENGINE_MEMSET( _jargs, 0, sizeof( jvalue ) * 32 );
        MENGINE_MEMSET( _jfree, 0, sizeof( jstring ) * 32 );

        uint32_t index_args = 0;
        uint32_t index_free = 0;

        for( const pybind::object & arg : _args )
        {
            if( arg.is_none() == true )
            {
                _jargs[index_args++].l = nullptr;

                MENGINE_STRCAT( signature, "L" );
            }
            else if( arg.is_bool() == true )
            {
                jboolean jvalue = (bool)arg.extract();
                _jargs[index_args++].z = jvalue;

                MENGINE_STRCAT( signature, "Z" );
            }
            else if( arg.is_integer() == true )
            {
                jint jvalue = (int32_t)arg.extract();
                _jargs[index_args++].i = jvalue;

                MENGINE_STRCAT( signature, "I" );
            }
            else if( arg.is_long() == true )
            {
                jlong jvalue = (int64_t)arg.extract();
                _jargs[index_args++].j = jvalue;

                MENGINE_STRCAT( signature, "J" );
            }
            else if( arg.is_float() == true )
            {
                jfloat jvalue = (float)arg.extract();
                _jargs[index_args++].f = jvalue;

                MENGINE_STRCAT( signature, "F" );
            }
            else if( arg.is_string() == true )
            {
                const Char * value_str = (const Char *)arg.extract();

                jstring jvalue = _jenv->NewStringUTF( value_str );

                _jargs[index_args++].l = jvalue;
                _jfree[index_free++] = jvalue;

                MENGINE_STRCAT( signature, "Ljava/lang/String;" );
            }
            else if( arg.is_list() == true )
            {
                pybind::list l = arg.extract();

                pybind::list::size_type s = l.size();

                jclass jclass_ArrayList = _jenv->FindClass( "java/util/ArrayList" );

                jmethodID jmethodID_List_constructor = _jenv->GetMethodID( jclass_ArrayList, "<init>", "(I)V" );

                MENGINE_ASSERTION_FATAL( jmethodID_List_constructor != nullptr, "invalid get android method 'java/lang/ArrayList [<init>] (I)V'" );

                jobject jlist = _jenv->NewObject( jclass_ArrayList, jmethodID_List_constructor, (jint)s);

                uint32_t index = 0;
                for( const pybind::object & o : l )
                {
                    const Char * o_str = o.extract();

                    jstring jelement = _jenv->NewStringUTF( o_str );

                    jmethodID jmethodID_List_add = _jenv->GetMethodID( jclass_ArrayList, "add", "(Ljava/lang/Object;)Z" );

                    MENGINE_ASSERTION_FATAL( jmethodID_List_add != nullptr, "invalid get android method 'java/lang/ArrayList [add] (Ljava/lang/Object;)Z'" );

                    jboolean result = _jenv->CallBooleanMethod( jlist, jmethodID_List_add, jelement );

                    Helper::jEnvExceptionCheck( _jenv );

                    _jenv->DeleteLocalRef( jelement );

                    ++index;
                }

                _jargs[index_args++].l = jlist;
                _jfree[index_free++] = jlist;

                _jenv->DeleteLocalRef( jclass_ArrayList );

                MENGINE_STRCAT( signature, "Ljava/util/List;" );
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

        MENGINE_STRCAT( signature, ")" );
        MENGINE_STRCAT( signature, _retType );

        jmethodID jmethodId = _jenv->GetMethodID( plugin_class, _method.c_str(), signature );

        if( jmethodId == nullptr )
        {
            Helper::jEnvExceptionCheck( _jenv );

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
    void AndroidNativePythonService::waitAndroidSemaphore( const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
    {
        LOGGER_INFO( "android", "wait android semaphore '%s' cb '%s' args '%s'"
            , _name.c_str()
            , _cb.repr().c_str()
            , _args.repr().c_str()
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return;
        }

        AndroidSemaphoreListenerDesc desc;
        desc.name = _name;
        desc.cb = _cb;
        desc.args = _args;

        m_semaphoreListeners.emplace_back( desc );

        jmethodID jmethodID_waitAndroidSemaphore = ANDROID_ENVIRONMENT_SERVICE()
                ->getActivityMethodID( jenv, "waitAndroidSemaphore", "(Ljava/lang/String;)V" );

        const Char * name_str = _name.c_str();

        jstring name_jvalue = jenv->NewStringUTF( name_str );

        ANDROID_ENVIRONMENT_SERVICE()
                ->callVoidActivityMethod( jenv, jmethodID_waitAndroidSemaphore, name_jvalue );

        jenv->DeleteLocalRef( name_jvalue );

        ANDROID_ENVIRONMENT_SERVICE()
            ->invokeAndroidEventations();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonService::activateSemaphore( const String & _name )
    {
        for( const AndroidSemaphoreListenerDesc & desc : m_semaphoreListeners )
        {
            if( desc.name != _name.c_str() )
            {
                continue;
            }

            desc.cb.call_args( desc.args );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
